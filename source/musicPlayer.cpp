#include "../header/MusicPlayer.hpp"
#include <iostream>
#include <algorithm>  // Include this for std::shuffle

MusicPlayer::MusicPlayer(const std::vector<std::string>& files)
    : musicFiles(files), currentIndex(0), isShuffled(false), isLooping(false), rng(std::random_device{}()) {
    for (size_t i = 0; i < musicFiles.size(); ++i) {
        shuffledIndices.push_back(i);
    }
    // Do not load or play any music here
}

void MusicPlayer::play() {
    if (music.getStatus() != sf::SoundSource::Playing) {
        music.play();
        startedPlaying = true;
    }
}

void MusicPlayer::pause() {
    music.pause();
}

void MusicPlayer::next() {
    if (isShuffled) {
        auto it = std::find(shuffledIndices.begin(), shuffledIndices.end(), currentIndex);
        if (it != shuffledIndices.end()) {
            currentIndex = shuffledIndices[(std::distance(shuffledIndices.begin(), it) + 1) % shuffledIndices.size()];
        }
    }
    else {
        currentIndex = (currentIndex + 1) % musicFiles.size();
    }

    if (!music.openFromFile(musicFiles[currentIndex])) {
        std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
    }
    music.setLoop(isLooping);
    music.play();
    
}

void MusicPlayer::previous() {
    if (!isLooping) {
        if (isShuffled) {
            size_t currentPosition = std::find(shuffledIndices.begin(), shuffledIndices.end(), currentIndex) - shuffledIndices.begin();
            currentIndex = shuffledIndices[(currentPosition - 1 + shuffledIndices.size()) % shuffledIndices.size()];
        }
        else {
            currentIndex = (currentIndex - 1 + musicFiles.size()) % musicFiles.size();
        }
    }

    if (!music.openFromFile(musicFiles[currentIndex])) {
        std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
    }
    music.setLoop(isLooping);
    music.play();
}

bool MusicPlayer::isCurrentSongFinished() const {
    return music.getStatus() == sf::SoundSource::Stopped;
}

bool MusicPlayer::hasStartedPlaying() const {
    return startedPlaying;
}

void MusicPlayer::setHasStartedPlaying(bool hasStarted) {
    startedPlaying = hasStarted;
}
void MusicPlayer::shufflePlaylist() {
    std::shuffle(shuffledIndices.begin(), shuffledIndices.end(), rng);
}

void MusicPlayer::shuffle(bool on) {
    isShuffled = on;
    if (isShuffled) {
        shufflePlaylist();
    }
}

void MusicPlayer::loop(bool on) {
    isLooping = on;
    music.setLoop(isLooping);
}

void MusicPlayer::playSong(size_t index) {
    if (index < musicFiles.size()) {
        currentIndex = index;
        if (isShuffled) {
            shuffledIndices.erase(std::remove(shuffledIndices.begin(), shuffledIndices.end(), index), shuffledIndices.end());
            shuffledIndices.insert(shuffledIndices.begin(), index);
        }
        if (!music.openFromFile(musicFiles[currentIndex])) {
            std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
        }
        music.setLoop(isLooping);
    }
}

sf::SoundSource::Status MusicPlayer::getStatus() const {
    return music.getStatus();
}

size_t MusicPlayer::getCurrentIndex() const {
    return currentIndex;
}

bool MusicPlayer::getIsShuffled() const {
    return isShuffled;
}

bool MusicPlayer::getIsLooping() const {
    return isLooping;
}

std::string MusicPlayer::getCurrentSong() const {
    return musicFiles[currentIndex];
}

float MusicPlayer::getTotalDuration() const {
        if (music.getDuration().asSeconds() > 0) {
            return music.getDuration().asSeconds();
        }
        return 0.f;  // Return 0 if no valid duration
    }

float MusicPlayer::getPlaybackPosition() const {
        if (music.getStatus() == sf::Music::Playing || music.getStatus() == sf::Music::Paused) {
            return music.getPlayingOffset().asSeconds();
        }
        return 0.f;
    }

float MusicPlayer::getPlaybackPercentage() const {
        float duration = getTotalDuration();
        if (duration > 0) {
            return getPlaybackPosition() / duration;
        }
        return 0.f;
    }

void MusicPlayer::setPlaybackPosition(float position) {
        if (position >= 0 && position <= getTotalDuration()) {
            music.setPlayingOffset(sf::seconds(position));
        }
    }

float MusicPlayer::getVolume() const {
    return music.getVolume();
}

void MusicPlayer::setVolume(float volume) {
    music.setVolume(volume);
}