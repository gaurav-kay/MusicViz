#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <limits>
#include <fftw3.h>
#include <thread>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <thread>

struct data {
    double idx;
    double freq;
    double mag;
    double mel;
};


// in this approach, video depends on audio, unlike prev where audio depended on video. so audio -> video now.
int main() {
    // use a custom stream class in the future, to only load chunk of data and fft on that chunk only. reducing memory usage
    // https://www.sfml-dev.org/documentation/3.0.2/classsf_1_1SoundStream.html#:~:text=data%20between%20threads.-,Usage%20example,-%3A
    
    // std::string filename = "../audio/freq_slide.ogg";
    // std::string filename = "../audio/flume_skin_preview.ogg";
    std::string filename = "../audio/ergo_proxy_whispa.ogg";

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename)) {
        printf("unable to load");
        return -1;
    }

    int sample_rate = buffer.getSampleRate();
    int sample_count = buffer.getSampleCount();
    int channel_count = buffer.getChannelCount();
    // int sampleCount = 2 * sampleRate;  // sec
    const sf::Int16 *samples_ptr = buffer.getSamples();
    // const sf::Int16 *samples_ptr = buffer.getSamples() + sampleRate;  // 1 sec headstart
    const float sample_window_length_in_seconds = 0.1;
    const auto display_update_frequency_in_ms = 100;
    const int sample_window_length = sample_rate * sample_window_length_in_seconds;
    std::vector<fftw_complex> out_complex(sample_window_length);
    std::vector<double> samples_norm(sample_window_length);
    std::vector<sf::Int16> samples(sample_window_length);

    fftw_plan plan_forward = fftw_plan_dft_r2c_1d(samples_norm.size(), samples_norm.data(), out_complex.data(), FFTW_ESTIMATE);

    sf::RenderWindow window(sf::VideoMode(2000, 600), "My window");
    window.setFramerateLimit(200);
    window.setVerticalSyncEnabled(true);  // v imp

    sf::Sound music(buffer);
    music.setVolume(20.0);
    music.play();

    // allocations:
    std::vector<data> output(sample_window_length);  
    sf::Event event;
    std::vector<sf::Vertex> line;

    long long currPos;
    long long currentTick = 0;
    long long prevTick = -1;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        currPos = music.getPlayingOffset().asMilliseconds();
        if (prevTick == -1) {
            prevTick = (long long) (currPos / display_update_frequency_in_ms);
        }

        currentTick = (long long) (currPos / display_update_frequency_in_ms);

        // if ((currPos - startPos) / display_update_frequency_in_ms < tick) {
        if (currentTick <= prevTick) {
            // sf::sleep(sf::milliseconds(5)); 
            window.display();
            continue;
        } 
        prevTick = currentTick;

        window.clear(sf::Color::Black);

        long sample_position = (currPos / 1000.0) * sample_rate * channel_count;
        // long sample_position = tick * sample_rate / display_update_frequency_in_ms;
        std::cout << "currPos in ms: " << currPos / 1000.0 << " sample_pos: " << sample_position << " to " << sample_position + sample_window_length << "\n";  // << " new_calc: " << tick * sample_rate / display_update_frequency_in_ms << "\n";
        // samples.assign(samples_ptr + sample_position, samples_ptr + sample_position + sample_window_length);
        for (int i = 0; i < sample_window_length; i++) {
           samples[i] = samples_ptr[sample_position + i * channel_count];
        }

        // for (size_t i = 0; i < samples.size(); i++) {
        for (size_t i = 0; i < sample_window_length; i++) {
            samples_norm[i] = samples[i] / (double) std::numeric_limits<sf::Int16>::max();
            // samples_norm[i] = samples[i] / (double) *std::max_element(samples.begin(), samples.end());
        }

        fftw_execute(plan_forward);

        for (size_t i = 0; i < sample_window_length; i++) {
            output[i].idx = (i + 1);
            output[i].freq = 44100 * i / sample_window_length;
            output[i].mag = sqrt(out_complex[i][0] * out_complex[i][0] + out_complex[i][1] * out_complex[i][1]);
        }

        line.clear();
        for (auto i : output) {
            line.push_back(
                sf::Vertex{
                    sf::Vector2f(
                        (i.idx / sample_window_length) * window.getSize().x * 4, // this is correct. i think x2 cus output of FFT half is missing. and x4 cus only 1st half is significant

                        i.mag * 5.0f
                    )
                }
            );
        }
        
        window.draw(line.data(), line.size(), sf::PrimitiveType::LineStrip);

        ///////////////////////

        // end the current frame
        window.display();

    }

    return 0;
}