#include <iostream>
#include <SFML/Audio.hpp>


int main() {
    // sf::Music music("music.ogg");

    // const std::filesystem::path filename = "ergo_proxy_whispa.mp3";

    sf::Music music;
    if (!music.openFromFile("ergo_proxy_whispa.ogg")) {
        printf("sdadsa");
        return -1; // error
    }

    printf("playing");
    music.play();

    while (music.getStatus() == sf::Music::Status::Playing) {
        // Leave some CPU time for other processes
        sf::sleep(sf::milliseconds(100));

        // Display the playing position
        std::cout << "\rPlaying... " << music.getPlayingOffset().asSeconds() << " sec        " << std::flush;
    }

    std::cout << '\n' << std::endl;

    printf("done");
    return 0;
}