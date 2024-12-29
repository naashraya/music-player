#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "MusicPlayer.hpp"

enum class Page { Home, NowPlaying };

// New base class
class BaseGUI {
public:
    BaseGUI(sf::RenderWindow& window, MusicPlayer& player) : window(window), player(player) {}
    virtual ~BaseGUI() = default;

    virtual void handleEvents() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;

protected:
    sf::RenderWindow& window;
    MusicPlayer& player;
};

// Existing GUI class, now derived from BaseGUI
class GUI : public BaseGUI {
public:
    GUI(sf::RenderWindow& window, MusicPlayer& player);
    void handleEvents() override;
    void update() override;
    void draw() override;

private:
    // All existing private members and methods remain unchanged
    void initializeGUI();
    void handleMouseClick(const sf::Event::MouseButtonEvent& mouseButton);
    void handleMouseMove(const sf::Event::MouseMoveEvent& mouseMove);
    void handleMouseRelease(const sf::Event::MouseButtonEvent& mouseButton);
    void handleTextEntered(const sf::Event::TextEvent& text);
    void drawHomePage();
    void drawNowPlayingPage();
    void drawAnimationBars(const sf::Vector2f& position);
    void drawSearchCursor();
    void togglePlayPause();
    void toggleShuffle();
    void toggleLoop();
    void setProgressFromMouseClick(float mouseX);
    void setVolumeFromMouseClick(float mouseX);
    void activateSearchBar();
    void deactivateSearchBar();
    void updateProgressBar();
    void updateProgressBarPreview(float mouseX);
    void updateVolumeSliderPreview(float mouseX);
    void handleHomePageClick(const sf::Event::MouseButtonEvent& mouseButton);
    void drawOscilloscope();
    void drawSpectrum();
    void drawBars();
    void updateTimeDisplay();
    std::string formatTime(int seconds);

    Page currentPage;
    bool isSearchBarActive;
    int clickedSongIndex;

    // SFML objects
    sf::Font font;
    sf::Texture playTexture, pauseTexture, nextTexture, prevTexture, shuffleTexture, loopTexture, volumeTexture;
    sf::Sprite playPauseButton, nextButton, prevButton, shuffleButton, loopButton, volumeButton;
    sf::RectangleShape sidebar, contentArea, controlBar, searchBar, progressBar, progressFill, volumeSliderBackground, volumeFill;
    sf::Text search, searchText;
    std::vector<sf::Text> sidebarTexts;
    std::vector<sf::RectangleShape> animationBars;

    // Other member variables
    sf::Clock clock;
    sf::Clock cursorBlinkClock;
    std::string searchQuery;
    std::string currentSong;
    std::vector<std::string> filteredMusicFiles;
    static constexpr float barMaxHeight = 20.0f;
    float animationTime = 0.0f;

    // Time display
    sf::Text currentTimeText;
    sf::Text totalTimeText;
};