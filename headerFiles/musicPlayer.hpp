#ifndef MUSICPLAYER_HPP
#define MUSICPLAYER_HPP

#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

class MusicPlayer {
public:
    MusicPlayer(const std::vector<std::string>& files);
    const sf::Music& getMusic() const { return music; }
    void play();
    void pause();
    void next();
    void previous();
    void shufflePlaylist();
    void shuffle(bool on);
    void loop(bool on);
    void playSong(size_t index);
    bool isCurrentSongFinished() const;
    bool hasStartedPlaying() const;
    void setHasStartedPlaying(bool hasStarted);
    float getTotalDuration() const;
    float getPlaybackPosition() const;
    void setPlaybackPosition(float position);
    float getPlaybackPercentage() const;

    const std::vector<std::string>& getMusicFiles() const {
        return musicFiles;
    }

    sf::SoundSource::Status getStatus() const;
    size_t getCurrentIndex() const;
    bool getIsShuffled() const;
    bool getIsLooping() const;
    std::string getCurrentSong() const;
    float getVolume() const;
    void setVolume(float volume);

private:
    std::vector<std::string> musicFiles;
    std::vector<size_t> shuffledIndices;
    sf::Music music;
    size_t currentIndex;
    bool isShuffled;
    bool isLooping;
    std::mt19937 rng;
    bool startedPlaying = false;
};

#endif // MUSICPLAYER_HPP