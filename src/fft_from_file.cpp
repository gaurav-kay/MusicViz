#include <SFML/Audio.hpp>
#include <vector>
#include <limits>
#include <cmath>
#include <iostream>
#include <fftw3.h>
#include <thread>

int main() {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("ergo_proxy_whispa.ogg")) {
        printf("unable to load");
        return -1;
    }

    int sampleRate = buffer.getSampleRate();
    // int sampleCount = buffer.getSampleCount();
    // int sampleCount = 1 * sampleRate;
    int sampleCount = 5 * sampleRate;  // 10 sec
    const sf::Int16 *samples_ptr = buffer.getSamples() + 44100;  // 1 sec headstart
    std::vector<sf::Int16> samples;
    const float window_length_in_seconds = 0.1;
    const int window_length = sampleRate * window_length_in_seconds;
    std::vector<fftw_complex> out_complex(window_length);
    std::vector<double_t> samples_norm(window_length);

    fftw_plan plan_forward = fftw_plan_dft_r2c_1d(samples_norm.size(), samples_norm.data(), out_complex.data(), FFTW_ESTIMATE);

    int count = 0;
    // while (samples_ptr + window_length < samples_ptr + sampleCount) {
    while (count + window_length < sampleCount) {  // not correct logic
        samples.assign(samples_ptr + count, samples_ptr + count + window_length);

        for (size_t i = 0; i < samples.size(); i++) {
            samples_norm[i] = samples[i] / (double) std::numeric_limits<sf::Int16>::max();
        }

        fftw_execute(plan_forward);

        for (size_t i = 0; i < window_length; i++) {
            std::cout << 
                count << "," << 
                i << "," <<
                out_complex[i][0] << "," << 
                out_complex[i][1] << std::endl;
        }

        count += (window_length * 1);
    }
    
    return 0;
}