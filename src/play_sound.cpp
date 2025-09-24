#include <SFML/Audio.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <vector>
#include <cmath>
#include <iostream>

void keep_playing(sf::Sound& sound);

int main() {
	constexpr std::size_t SAMPLES = 44100 * 3;
	constexpr std::size_t SAMPLE_RATE = 44100;

	std::vector<std::int16_t> raw(SAMPLES); // using an std::vector keeps this large resource off the stack and stores it in the heap

	constexpr std::int16_t AMPLITUDE = 30000;
	constexpr double TWO_PI = 6.28318;
	constexpr double increment = 0.5 / 44100.0;
	double x = 0.0;
	for (std::size_t i = 0; i < SAMPLES; ++i)
	{
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
	sound.setLoop(true);
	sound.play();


    keep_playing(sound);

    return 0;
}

void keep_playing(sf::Sound& sound) {
    while (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) // we can quit by holding Escape
	{
        printf("checking\n");
		sf::sleep(sf::milliseconds(100));
	}

    // while (sound.getStatus() == sf::Music::Status::Playing) {
    //     // Leave some CPU time for other processes
    //     sf::sleep(sf::milliseconds(100));

    //     // Display the playing position
    //     std::cout << "\rPlaying... " << sound.getPlayingOffset().asSeconds() << " sec        " << std::flush;
    // }

    // std::cout << '\n' << std::endl;

}