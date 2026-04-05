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
#include <optional>

struct data {
    double idx;
    double freq;
    double mag;
    double mel;
    int mel_idx;
};


int main() {
    // std::string filename = "../audio/jingle.ogg";
    // std::string filename = "../audio/flume_skin_preview.ogg";
    // std::string filename = "../audio/flume_skin_preview_2.ogg";
    // std::string filename = "../audio/ruffsqwad.ogg";
    std::string filename = "../audio/cokestudio.ogg";
    // std::string filename = "../audio/flume_skin_preview_full.ogg";
    // std::string filename = "../audio/peekaboo.ogg";
    // std::string filename = "../audio/joshpan.ogg";
    // std::string filename = "../audio/ergo_proxy_whispa.ogg";
    // std::string filename = "../audio/Are_We_Dreaming_peekaboo.mp3";

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename)) {
        printf("unable to load");
        return -1;
    }

    int sample_rate = buffer.getSampleRate();
    int sample_count = buffer.getSampleCount();
    int channel_count = buffer.getChannelCount();
    const std::int16_t *samples_ptr = buffer.getSamples();
    const float sample_window_length_in_seconds = 0.05;
    const auto display_update_frequency_in_ms = 20;  // imp to have this and sample_window_length in relative sync, disp > window_length maybe
    const int sample_window_length = sample_rate * sample_window_length_in_seconds;
    std::vector<fftw_complex> out_complex(sample_window_length);
    std::vector<double> samples_norm(sample_window_length);
    std::vector<std::int16_t> samples(sample_window_length);

    fftw_plan plan_forward = fftw_plan_dft_r2c_1d(samples_norm.size(), samples_norm.data(), out_complex.data(), FFTW_ESTIMATE);

    sf::RenderWindow window(sf::VideoMode({2000, 600}), "Music Visualizer");
    window.setFramerateLimit(200);
    window.setVerticalSyncEnabled(true);

    sf::Music music(filename);
    music.setVolume(50.0);
    music.play();

    // allocations:
    std::vector<data> output(sample_window_length);
    std::vector<sf::Vertex> line;

    long long currPos;
    long long currentTick = 0;
    long long prevTick = -1;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        currPos = music.getPlayingOffset().asMilliseconds();
        if (prevTick == -1) {
            prevTick = (long long) (currPos / display_update_frequency_in_ms);
        }

        currentTick = (long long) (currPos / display_update_frequency_in_ms);
        if (currentTick <= prevTick) {
            // sf::sleep(sf::milliseconds(5)); 
            window.display();
            continue;
        } 
        prevTick = currentTick;

        window.clear(sf::Color::Black);

        long sample_position = (currPos / 1000.0) * sample_rate * channel_count;
        sample_position -= sample_window_length * channel_count / 2;  // centered at cursor
        for (int i = 0; i < sample_window_length; i++) {
           samples[i] = samples_ptr[sample_position + i * channel_count];
        }

        for (size_t i = 0; i < sample_window_length; i++) {
            samples_norm[i] = samples[i] / (double) std::numeric_limits<std::int16_t>::max();
        }

        fftw_execute(plan_forward);

        for (size_t i = 0; i < sample_window_length; i++) {
            output[i].idx = (i + 1);
            output[i].freq = 44100 * i / sample_window_length;
            output[i].mag = sqrt(out_complex[i][0] * out_complex[i][0] + out_complex[i][1] * out_complex[i][1]);

            if (output[i].freq < 1000) {
                output[i].mel = 3 * output[i].freq / 200.0;
                output[i].mel_idx = std::floor(output[i].mel * 10);
            } else {
                double mel = output[i].freq / 1000.0;
                mel = log(mel) / log(6.4);
                output[i].mel = 15 + 27 * mel;
                output[i].mel_idx = std::floor(output[i].mel * 10);
            }
        }

        // approach:
        // mel * 10, so min and max will be: max mel idx = 70.0711, so max mapped x axis value will be 700.

        // bin and aggregate mel_idx
        std::vector<double> output_data_xy(600, 0);
        std::vector<int> output_data_xy_number_of_bins(600, 0);
        for (auto i: output) {
            if (i.mel_idx < output_data_xy.size()) {
                output_data_xy[i.mel_idx] += i.mag;
                output_data_xy_number_of_bins[i.mel_idx] += 1;
            }
        }

        // avg the mel bins
        for (size_t i = 0; i < output_data_xy.size(); i++) {
            if (output_data_xy_number_of_bins[i] > 0) {
                output_data_xy[i] = output_data_xy[i] / (double) output_data_xy_number_of_bins[i];
            }
        }

        line.clear();
        for (int i = 0; i < output_data_xy.size(); i++) {
            line.push_back(
                sf::Vertex{
                    sf::Vector2f(
                        (int) ((i / (double) output_data_xy.size()) * window.getSize().x),
                        window.getSize().y - output_data_xy[i]
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