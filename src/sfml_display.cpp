#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <limits>
#include <fftw3.h>
#include <thread>
#include <iostream>
#include <math.h>
#include <algorithm>

struct data {
    double idx;
    double freq;
    double mag;
    double mel;
};

int main() {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("ergo_proxy_whispa.ogg")) {
        printf("unable to load");
        return -1;
    }

    int sampleRate = buffer.getSampleRate();
    int sampleCount = buffer.getSampleCount();
    // int sampleCount = 30 * sampleRate;  // sec
    const sf::Int16 *samples_ptr = buffer.getSamples();
    // const sf::Int16 *samples_ptr = buffer.getSamples() + sampleRate;  // 1 sec headstart
    const float sample_window_length_in_seconds = 0.1;
    const int sample_window_length = sampleRate * sample_window_length_in_seconds;
    std::vector<fftw_complex> out_complex(sample_window_length);
    std::vector<double> samples_norm(sample_window_length);
    std::vector<sf::Int16> samples(sample_window_length);

    fftw_plan plan_forward = fftw_plan_dft_r2c_1d(samples_norm.size(), samples_norm.data(), out_complex.data(), FFTW_ESTIMATE);

    sf::RenderWindow window(sf::VideoMode(1500, 500), "My window");
    window.setFramerateLimit(24);

    int count = 0;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear(sf::Color::Black);

        ///////////////////////

        if (count + sample_window_length > sampleCount) {
            window.display();
            continue;
        }

        samples.assign(samples_ptr + count, samples_ptr + count + sample_window_length);

        for (size_t i = 0; i < samples.size(); i++) {
            samples_norm[i] = samples[i] / (double) std::numeric_limits<sf::Int16>::max();
        }

        fftw_execute(plan_forward);

        std::vector<data> output(sample_window_length);  

        for (size_t i = 0; i < sample_window_length; i++) {
            output[i].idx = i;
            output[i].freq = 44100 * i / sample_window_length;
            output[i].mag = sqrt(out_complex[i][0] * out_complex[i][0] + out_complex[i][1] * out_complex[i][1]);
        }
        // need to know max value of fft output to do min-max norm
        // auto max = *std::max_element(output.begin(), output.end(), [&](const data &a, const data &b) {
        //     return a.mag > b.mag;
        // });
        // auto min = *std::min_element(output.begin(), output.end(), [&](const data &a, const data &b) {
        //     return a.mag < b.mag;
        // });
        // std::cout << max.mag << " " << min.mag << "\n"; 
        // for (size_t i = 0; i < sample_window_length; i++) {
        //     output[i].mag = (output[i].mag - min.mag) / (max.mag - min.mag);
        // }

        std::vector<sf::Vertex> line;
        for (auto i : output) {
            line.push_back(
                sf::Vertex{
                    sf::Vector2f(
                        (i.idx + 1) * sample_window_length * 0.4 / window.getSize().x,  //  place to fit whole fft in view
                        i.mag * 5.0f
                    )
                }
            );
        }

        // std::array line =
        // {
        //     sf::Vertex{sf::Vector2f(10.f, 10.f)},
        //     sf::Vertex{sf::Vector2f(150.f, 150.f)},
        //     sf::Vertex{sf::Vector2f(150.f, 250.f)},
        //     sf::Vertex{sf::Vector2f(170.f, 270.f)}
        // };

        // window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
        window.draw(line.data(), line.size(), sf::PrimitiveType::LineStrip);



        count += (sample_window_length * 1);

        ///////////////////////

        // end the current frame
        window.display();
    }

    return 0;
}