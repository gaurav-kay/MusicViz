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

void play_music(sf::SoundBuffer &buffer) {
// void play_music() {
    // sf::SoundBuffer buffer;
    // if (!buffer.loadFromFile("metronome_output.ogg")) {
    //     printf("unable to load");
    // }
    // std::cout << "playing\n";
    // sf::Sound sound(buffer);
    // sound.play();

    // while (sound.getStatus() == sf::Sound::Playing) {
    //     sf::sleep(sf::milliseconds(100));
    // }
}

int main() {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("../audio/sine_wave.ogg")) {
    // if (!buffer.loadFromFile("metronome_output.ogg")) {
        printf("unable to load");
        return -1;
    }

    int sampleRate = buffer.getSampleRate();
    int sampleCount = buffer.getSampleCount();
    // int sampleCount = 2 * sampleRate;  // sec
    const sf::Int16 *samples_ptr = buffer.getSamples();
    // const sf::Int16 *samples_ptr = buffer.getSamples() + sampleRate;  // 1 sec headstart
    const float sample_window_length_in_seconds = 0.1;
    const int sample_window_length = sampleRate * sample_window_length_in_seconds;
    std::vector<fftw_complex> out_complex(sample_window_length);
    std::vector<double> samples_norm(sample_window_length);
    std::vector<sf::Int16> samples(sample_window_length);

    fftw_plan plan_forward = fftw_plan_dft_r2c_1d(samples_norm.size(), samples_norm.data(), out_complex.data(), FFTW_ESTIMATE);

    sf::RenderWindow window(sf::VideoMode(2000, 600), "My window");
    // window.setFramerateLimit(24);

    // std::thread music_thread(play_music, std::ref(buffer));
    // std::thread music_thread(play_music);
    // play_music(buffer);

    std::cout << "playing\n";
    sf::Sound sound(buffer);
    // sf::Time t = sf::seconds(50);
    // sound.setPlayingOffset(t);
    sound.play();  // todo: keeping this and the drawing in sync is tougher than thought.

    // timing variables
    auto prev = std::chrono::high_resolution_clock::now() - std::chrono::milliseconds(10000);
    // std::cout << "prev = " << prev.time_since_epoch().count() << "\n";

    int count = 0;  // tracks progress of song, sampleNumber -> part of sampleCount
    while (window.isOpen()) {
        /**
         * if elapsed time > 100ms:
         *  go into loop, else wait (infitite spinner)
         * 
         * check elapsed time: if current - prev > 100:
         *   prev = current
         * 
         *
         */
        auto current = std::chrono::high_resolution_clock::now();
        auto difference = std::chrono::duration_cast<std::chrono::milliseconds>(current - prev).count();
        if (difference < 50) {  // how did 50 ms work -> why is it /2. Is it something like 100ms samples, -> drawing to 50 ms
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }
            window.clear(sf::Color::Black);

            // std::cout << "loopin\n";
            continue;
        } 
        // std::cout << "count " << count << "\n";
        // std::cout << "prev = " << prev.time_since_epoch().count() << "\n";
        // std::cout << "current   = " << current.time_since_epoch().count() << "\n";
        // std::cout << "difference = " << difference << " ms\n";
        // std::cout << "difference2 = " << current.time_since_epoch().count() - prev.time_since_epoch().count() << " ms\n";
        prev = current;


        // sf::Event event;
        // while (window.pollEvent(event)) {
        //     if (event.type == sf::Event::Closed) {
        //         window.close();
        //     }
        // }
        // window.clear(sf::Color::Black);

        ///////////////////////

        if (count + sample_window_length > sampleCount) {  // song reached end
            window.display();
            continue;
        }

        samples.assign(samples_ptr + count, samples_ptr + count + sample_window_length);

        // play sound
        // sf::SoundBuffer buffer{};
        // if (!buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate))
        // {
        //     std::cerr << "Loading failed!" << std::endl;
        //     exit(-1);
        // }

        // // std::cout << "playing original\n";
        // sf::Sound sound(buffer);
        // // sound.setLoop(true);
        // sound.play();

        ///

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
                        // i.idx * sample_window_length * 0.4 / window.getSize().x,  //  place to fit whole fft in view

                        (i.idx / sample_window_length) * window.getSize().x * 4, // this is correct. i think x2 cus output of FFT half is missing. and x4 cus only 1st half is significant

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