#include <SFML/Audio.hpp>
#include <vector>
#include <limits>
#include <cmath>
#include <iostream>
#include <fftw3.h>
#include <thread>

int main() {
    // sf::SoundBuffer buffer;
    // if (!buffer.loadFromFile("ergo_proxy_whispa.ogg")) {
    //     printf("unable to load");
    //     return -1;
    // }

    // int sampleRate = buffer.getSampleRate();
    // int sampleCount = buffer.getSampleCount();
    // const sf::Int16 *samples = buffer.getSamples();

    // sf::Int16 min = std::numeric_limits<sf::Int16>::max();
    // sf::Int16 max = std::numeric_limits<sf::Int16>::min();
    // sf::Int16 avg = 0;
    // for (int i = 0; i < sampleCount; i++) {
    //     // std::cout << samples[i * 100] << " ";
    //     // if (samples[i] < min) {
    //     //     std::cout << "min found at " << i << ": " << samples[i] << std::endl;
    //     // }
    //     min = samples[i] < min ? samples[i] : min;
    //     // if (samples[i] > max) {
    //     //     std::cout << "max found at " << i << ": " << samples[i] << std::endl;
    //     // }
    //     max = samples[i] > max ? samples[i] : max;
    //     avg = ((avg * (i - 1)) + samples[i]) / (i + 1);
    // }
    // // std::cout << min << " " << max << " " << avg << std::endl;



	constexpr std::size_t SAMPLES = 44100 * 3;
	// constexpr std::size_t SAMPLES = 10000;
	constexpr std::size_t SAMPLE_RATE = 44100;

	std::vector<std::int16_t> raw(SAMPLES); // using an std::vector keeps this large resource off the stack and stores it in the heap

	constexpr std::int16_t AMPLITUDE = 30000;
	constexpr double TWO_PI = 6.28318;
	constexpr double increment = 1000 / 44100.0;
	double x = 0.0;
	for (std::size_t i = 0; i < SAMPLES; ++i) {
		raw[i] = static_cast<std::int16_t>(AMPLITUDE * sin(x * TWO_PI));
		x += increment;
	}

    sf::SoundBuffer buffer{};
	if (!buffer.loadFromSamples(raw.data(), raw.size(), 1, SAMPLE_RATE))
	{
		std::cerr << "Loading failed!" << std::endl;
		return EXIT_FAILURE;
	}

	sf::Sound sound(buffer);
	// sound.setLoop(true);
	sound.play();

    // std::this_thread::sleep_for(std::chrono::seconds(SAMPLES / SAMPLE_RATE));



    std::vector<std::double_t> in(raw.size());
    for (size_t i = 0; i < SAMPLES; i++) {
        in[i] = raw[i] / (double) std::numeric_limits<std::int16_t>::max();
        // std::cout << raw[i] << " -> " << in[i] << "\n";
    }
    std::cout << std::endl;

    // real to complex or whatever.
    fftw_complex* out = (fftw_complex *) fftw_malloc((SAMPLES / 2) * sizeof(fftw_complex));

    
    fftw_plan plan = fftw_plan_dft_r2c_1d(SAMPLES, in.data(), out, FFTW_ESTIMATE);
    
    fftw_execute(plan);

    for (size_t i = 0; i < (SAMPLES / 2); i++) {
        std::cout << i << "," << out[i][0] << "," << out[i][1] << std::endl; 
    }
    

    return 0;
}