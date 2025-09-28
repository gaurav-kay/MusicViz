// just tests to see if you can reconstruct a signal by fft-ing forward and backward
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
    // std::vector<sf::Int16> samples_vec;
    // samples_vec.assign(samples, samples + sampleCount);

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


    ///////// SINE WAVE
	constexpr std::size_t SAMPLES = 5 * 44100;
	// constexpr std::size_t SAMPLES = 10000;
	constexpr std::size_t SAMPLE_RATE = 44100;

	std::vector<std::int16_t> raw(SAMPLES); // using an std::vector keeps this large resource off the stack and stores it in the heap

	constexpr std::int16_t AMPLITUDE = 30000;
	constexpr double TWO_PI = 6.28318;
	constexpr double increment = 500 / 44100.0;
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

    // std::cout << "playing original\n";
	sf::Sound sound(buffer);
	// sound.setLoop(true);
	sound.play();

    std::this_thread::sleep_for(std::chrono::seconds(SAMPLES / SAMPLE_RATE));


    // NORM
    // std::vector<std::double_t> in(20 * 44100);
    std::vector<fftw_complex> in_complex(SAMPLES);
    std::vector<fftw_complex> out_complex(SAMPLES);


    fftw_plan plan_forward = fftw_plan_dft_1d(in_complex.size(), in_complex.data(), out_complex.data(), FFTW_FORWARD, FFTW_MEASURE);
    fftw_plan plan_backward = fftw_plan_dft_1d(out_complex.size(), out_complex.data(), in_complex.data(), FFTW_BACKWARD, FFTW_MEASURE);



    // for (size_t i = 0; i < in.size(); i++) {
    for (size_t i = 0; i < raw.size(); i++) {
        // in[i] = samples[i] / (double) std::numeric_limits<sf::Int16>::max();
        // in_complex[i][0] = samples[i] / (double) std::numeric_limits<sf::Int16>::max();
        in_complex[i][0] = raw[i] / (double) std::numeric_limits<sf::Int16>::max();
        // in_complex[i][0] = raw[i];  // this shit is needed
        in_complex[i][1] = 0.0;
        std::cout << raw[i] << "\n";
    }
    std::cout << "_\n";

    // fftw_complex* out = (fftw_complex *) fftw_malloc((in.size() / 2 + 1) * sizeof(fftw_complex));
    // fftw_complex* out = (fftw_complex *) fftw_malloc((in.size()) * sizeof(fftw_complex));
    
    // fftw_plan plan_forward = fftw_plan_dft_r2c_1d(in.size(), in.data(), out, FFTW_ESTIMATE);
    // fftw_plan plan_forward = fftw_plan_dft_1d(in_complex.size(), in_complex.data(), out_complex.data(), FFTW_FORWARD, FFTW_MEASURE);
    
    fftw_execute(plan_forward);

    // for (size_t i = 0; i < (in.size() / 2) + 1; i++) {
    //     std::cout << i << "," << out[i][0] << "," << out[i][1] << std::endl; 
    // }

    // remove high freq things
    // for (size_t i = 0; i < (in.size() / 4); i++) {
    //     out[i][0] = 0;
    //     out[i][1] = 0;
    // }
    
    // fftw_plan plan_backward = fftw_plan_dft_c2r_1d((in.size() / 2) + 1, out, in.data(), FFTW_ESTIMATE);
    // fftw_plan plan_backward = fftw_plan_dft_1d(out_complex.size(), out_complex.data(), in_complex.data(), FFTW_BACKWARD, FFTW_MEASURE);


    fftw_execute(plan_backward);
    


    // std::vector<sf::Int16> output_play(in.size());
    std::vector<sf::Int16> output_play(in_complex.size());
    // for (size_t i = 0; i < in.size(); i++) {
    for (size_t i = 0; i < in_complex.size(); i++) {
        // output_play[i] = static_cast<sf::Int16>(in[i]);
        output_play[i] = static_cast<sf::Int16>(in_complex[i][0]);
        std::cout << in_complex[i][0] << "\n";
    }

    sf::SoundBuffer buffer2{};
	if (!buffer2.loadFromSamples(output_play.data(), output_play.size(), 1, SAMPLE_RATE))
	{
		std::cerr << "Loading failed!" << std::endl;
		return EXIT_FAILURE;
	}

    // std::cout << "playing\n";
	sf::Sound sound2(buffer2);
	// sound.setLoop(true);
	sound2.play();

    std::this_thread::sleep_for(std::chrono::seconds(output_play.size() / SAMPLE_RATE));

    

    return 0;
}