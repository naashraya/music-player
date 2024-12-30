
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "../header/MusicPlayer.hpp"
#include "../header/GUI.hpp"
#include "../header/Utilities.hpp"
#include <algorithm>

namespace fs = std::filesystem;

std::vector<std::string> getSongsFromDirectory(const std::string& directoryPath) {
    std::vector<std::string> musicFiles;
    
    for (const auto & entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            if (extension == ".mp3" || extension == ".wav" || extension == ".ogg") {
                musicFiles.push_back(entry.path().string());
            }
        }
    }
    
    return musicFiles;
}

int main() {
    // Get desktop mode and reduce height by a bit to avoid overlapping the taskbar
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(desktopMode.width - 3, desktopMode.height - 90), "SFML Music Player", sf::Style::Default);

    // Get the songs from the Songs directory
    std::string songsDirectory = "../Songs";
    std::vector<std::string> musicFiles = getSongsFromDirectory(songsDirectory);

    if (musicFiles.empty()) {
        std::cerr << "No music files found in the Songs directory." << std::endl;
        return 1;
    }

    // Create the music player
    MusicPlayer player(musicFiles);

    GUI gui(window, player);

    // Start the game loop
    while (window.isOpen()) {
        gui.handleEvents();
        gui.update();
        gui.draw();
    }

    return 0;
}
