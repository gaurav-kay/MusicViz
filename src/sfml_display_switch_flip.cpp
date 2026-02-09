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
    
    sf::SoundBuffer buffer;
    // if (!buffer.loadFromFile("../audio/sine_wave.ogg")) {
    if (!buffer.loadFromFile("../audio/ergo_proxy_whispa.ogg")) {
    // if (!buffer.loadFromFile("metronome_output.ogg")) {
        printf("unable to load");
        return -1;
    }

    int sample_rate = buffer.getSampleRate();
    int sample_count = buffer.getSampleCount();
    // int sampleCount = 2 * sampleRate;  // sec
    const sf::Int16 *samples_ptr = buffer.getSamples();
    // const sf::Int16 *samples_ptr = buffer.getSamples() + sampleRate;  // 1 sec headstart
    const float sample_window_length_in_seconds = 0.1;
    const auto display_update_frequency_in_ms = 1000;
    const int sample_window_length = sample_rate * sample_window_length_in_seconds;
    std::vector<fftw_complex> out_complex(sample_window_length);
    std::vector<double> samples_norm(sample_window_length);
    std::vector<sf::Int16> samples(sample_window_length);

    fftw_plan plan_forward = fftw_plan_dft_r2c_1d(samples_norm.size(), samples_norm.data(), out_complex.data(), FFTW_ESTIMATE);

    sf::RenderWindow window(sf::VideoMode(2000, 600), "My window");
    window.setFramerateLimit(24);

    std::cout << "playing\n";
    // sf::Sound sound(buffer);
    // sf::Time t = sf::seconds(50);
    // sound.setPlayingOffset(t);
    // sound.play();  // todo: keeping this and the drawing in sync is tougher than thought.

    sf::Music music;
    music.openFromFile("../audio/ergo_proxy_whispa.ogg");
    music.play();

    while (true) {}

    int count = 0;
    long currPos;
    while (false) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        currPos = music.getPlayingOffset().asMilliseconds();
        if (currPos % display_update_frequency_in_ms != 0) {
            continue;
        }
        std::cout << "displaying" << currPos << "\n"; 
        window.clear(sf::Color::Black);

        // do operation

\        samples.assign(samples_ptr + count, samples_ptr + count + sample_window_length);

        for (size_t i = 0; i < samples.size(); i++) {
            samples_norm[i] = samples[i] / (double) std::numeric_limits<sf::Int16>::max();
        }

        fftw_execute(plan_forward);

        std::vector<data> output(sample_window_length);  

        for (size_t i = 0; i < sample_window_length; i++) {
            output[i].idx = (i + 1);
            output[i].freq = 44100 * i / sample_window_length;
            output[i].mag = sqrt(out_complex[i][0] * out_complex[i][0] + out_complex[i][1] * out_complex[i][1]);
        }

        std::vector<sf::Vertex> line;
        for (auto i : output) {
            line.push_back(
                sf::Vertex{
                    sf::Vector2f(
                        // i.idx * sample_window_length * 0.4 / window.getSize().x,  //  place to fit whole fft in view

                        (i.idx / sample_window_length) * window.getSize().x * 4, // this is correct. i think x2 cus output of FFT half is missing. and x4 cus only 1st half is significant

                        i.mag * 5.0f
                    )
                }
            );
        }
        
        // window.clear(sf::Color::Black);

        window.draw(line.data(), line.size(), sf::PrimitiveType::LineStrip);

        count += (sample_window_length * 1);

        ///////////////////////

        // end the current frame
        window.display();

    }

    return 0;
}