#include "../header/GUI.hpp"
#include "../header/Utilities.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

GUI::GUI(sf::RenderWindow& window, MusicPlayer& player)
    : BaseGUI(window, player), currentPage(Page::Home), isSearchBarActive(false), clickedSongIndex(-1) {
    initializeGUI();
}

void GUI::initializeGUI() {
    // Load textures and set up sprites
    if (!playTexture.loadFromFile("../Icons/play.png") ||
        !pauseTexture.loadFromFile("../Icons/pause.png") ||
        !nextTexture.loadFromFile("../Icons/skip.png") ||
        !prevTexture.loadFromFile("../Icons/back.png") ||
        !shuffleTexture.loadFromFile("../Icons/shuffle.png") ||
        !loopTexture.loadFromFile("../Icons/loop.png") ||
        !volumeTexture.loadFromFile("../Icons/volume.png")) {
        std::cerr << "Error loading images" << std::endl;
    }

    // Set up sprites
    playPauseButton.setTexture(playTexture);
    nextButton.setTexture(nextTexture);
    prevButton.setTexture(prevTexture);
    shuffleButton.setTexture(shuffleTexture);
    loopButton.setTexture(loopTexture);
    volumeButton.setTexture(volumeTexture);

    // Set button sizes and positions
    float buttonWidth = 80.0f;
    float buttonHeight = 80.0f;
    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;
    float yPosition = windowHeight - buttonHeight - 10.0f;

    playPauseButton.setScale(buttonWidth / playPauseButton.getLocalBounds().width, buttonHeight / playPauseButton.getLocalBounds().height);
    nextButton.setScale(buttonWidth / nextButton.getLocalBounds().width, buttonHeight / nextButton.getLocalBounds().height);
    prevButton.setScale(buttonWidth / prevButton.getLocalBounds().width, buttonHeight / prevButton.getLocalBounds().height);
    shuffleButton.setScale(buttonWidth / shuffleButton.getLocalBounds().width, buttonHeight / shuffleButton.getLocalBounds().height);
    loopButton.setScale(buttonWidth / loopButton.getLocalBounds().width, buttonHeight / loopButton.getLocalBounds().height);
    volumeButton.setScale(buttonWidth / volumeButton.getLocalBounds().width, buttonHeight / volumeButton.getLocalBounds().height);

    playPauseButton.setPosition((windowWidth - buttonWidth) / 2, yPosition);
    prevButton.setPosition(playPauseButton.getPosition().x - 2 * buttonWidth, yPosition);
    nextButton.setPosition(playPauseButton.getPosition().x + 2 * buttonWidth, yPosition);
    shuffleButton.setPosition(playPauseButton.getPosition().x - 4 * buttonWidth, yPosition);
    loopButton.setPosition(playPauseButton.getPosition().x + 4 * buttonWidth, yPosition);
    volumeButton.setPosition(10, yPosition);

    // Define new widths
    float sidebarWidth = 300.0f; // New width for the sidebar

    // Set up shapes
    sidebar.setSize(sf::Vector2f(sidebarWidth, windowHeight));
    sidebar.setFillColor(sf::Color(30, 30, 30));

    contentArea.setSize(sf::Vector2f(windowWidth - sidebarWidth, windowHeight));
    contentArea.setPosition(sidebarWidth, 0.0f);
    contentArea.setFillColor(sf::Color(40, 40, 40));

    controlBar.setSize(sf::Vector2f(windowWidth, 100.0f));
    controlBar.setPosition(0.0f, windowHeight - 100.0f);
    controlBar.setFillColor(sf::Color(20, 20, 20));

    searchBar.setSize(sf::Vector2f(280.0f, 40.0f)); // Increased width for the search bar
    searchBar.setPosition(10.0f, 10.0f);
    searchBar.setFillColor(sf::Color(50, 50, 50));

    float progressBarWidth = window.getSize().x - sidebarWidth - 140.0f; // Reduced width
    progressBar.setSize(sf::Vector2f(progressBarWidth, 15.0f));
    progressBar.setPosition(sidebarWidth + 70.0f, windowHeight - 135.0f);
    progressBar.setFillColor(sf::Color(60, 60, 60));

    progressFill.setSize(sf::Vector2f(0, 15.0f));
    progressFill.setPosition(progressBar.getPosition());
    progressFill.setFillColor(sf::Color(29, 185, 84));

    volumeSliderBackground.setSize(sf::Vector2f(120.0f, 10.0f));
    volumeSliderBackground.setPosition(120.0f, windowHeight - 53.0f);
    volumeSliderBackground.setFillColor(sf::Color(60, 60, 60));

    volumeFill.setSize(sf::Vector2f(80.0f, 10.0f));
    volumeFill.setPosition(120.0f, windowHeight - 53.0f);
    volumeFill.setFillColor(sf::Color(29, 185, 84));

    // Load font
    if (!font.loadFromFile("../Fonts/ARIAL.ttf")) {
        std::cerr << "Error loading font" << std::endl;
    }
    currentTimeText.setFont(font);
    currentTimeText.setCharacterSize(20);
    currentTimeText.setFillColor(sf::Color::White);

    totalTimeText.setFont(font);
    totalTimeText.setCharacterSize(20);
    totalTimeText.setFillColor(sf::Color::White);

    updateTimeDisplay();

    // Set up text
    search.setFont(font);
    search.setString("Search");
    search.setCharacterSize(23);
    search.setFillColor(sf::Color(200, 200, 200));
    search.setPosition(15.0f, 15.0f);

    searchText.setFont(font);
    searchText.setCharacterSize(25);
    searchText.setFillColor(sf::Color::White);
    searchText.setPosition(15.0f, 15.0f);

    // Set up sidebar texts
    std::vector<std::string> sidebarOptions = { "Home", "Now\nPlaying" };
    for (size_t i = 0; i < sidebarOptions.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(sidebarOptions[i]);
        text.setCharacterSize(40);
        text.setFillColor(sf::Color::White);
        text.setPosition(20.0f, 60.0f + i * 130.0f);
        sidebarTexts.push_back(text);
    }

    // Initialize animation bars
    for (int i = 0; i < 3; ++i) {
        sf::RectangleShape bar;
        bar.setSize(sf::Vector2f(5.0f, 0.0f));
        bar.setFillColor(sf::Color(29, 185, 84));
        animationBars.push_back(bar);
    }

    currentSong = ""; // Initialize with empty string
    playPauseButton.setTexture(playTexture);
}

void GUI::updateTimeDisplay() {
        if (player.hasStartedPlaying()) {
            float totalDuration = player.getTotalDuration();
            float currentPosition = player.getPlaybackPosition();

            int currentSeconds = static_cast<int>(currentPosition);
            int totalSeconds = static_cast<int>(totalDuration);

            currentTimeText.setString(formatTime(currentSeconds));
            totalTimeText.setString(formatTime(totalSeconds));

            // Position the time texts
            currentTimeText.setPosition(progressBar.getPosition().x - currentTimeText.getLocalBounds().width - 10,
                progressBar.getPosition().y - currentTimeText.getLocalBounds().height - 5);
            totalTimeText.setPosition(progressBar.getPosition().x + progressBar.getSize().x + 10,
                progressBar.getPosition().y - totalTimeText.getLocalBounds().height - 5);
        } 
}


std::string GUI::formatTime(int seconds) {
    int minutes = seconds / 60;
    int remainingSeconds = seconds % 60;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << remainingSeconds;
    return ss.str();
}

void GUI::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            handleMouseClick(event.mouseButton);
        }
        else if (event.type == sf::Event::MouseMoved) {
            handleMouseMove(event.mouseMove);
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            handleMouseRelease(event.mouseButton);
        }
        else if (event.type == sf::Event::TextEntered && isSearchBarActive) {
            handleTextEntered(event.text);
        }
    }
}

void GUI::handleMouseClick(const sf::Event::MouseButtonEvent& mouseButton) {
    if (playPauseButton.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        togglePlayPause();
    }
    else if (nextButton.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        player.next();
        playPauseButton.setTexture(pauseTexture);
        currentSong = getBaseName(player.getCurrentSong());
        clickedSongIndex = player.getCurrentIndex();
    }
    else if (prevButton.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        player.previous();
        playPauseButton.setTexture(pauseTexture);
        currentSong = getBaseName(player.getCurrentSong());
        clickedSongIndex = player.getCurrentIndex();
    }
    else if (shuffleButton.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        toggleShuffle();
    }
    else if (loopButton.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        toggleLoop();
    }
    else if (sidebarTexts[0].getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        currentPage = Page::Home;
    }
    else if (sidebarTexts[1].getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        currentPage = Page::NowPlaying;
    }
    else if (progressBar.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        setProgressFromMouseClick(mouseButton.x);
    }
    else if (volumeSliderBackground.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        setVolumeFromMouseClick(mouseButton.x);
    }
    else if (searchBar.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        activateSearchBar();
    }
    else {
        deactivateSearchBar();
    }

    if (currentPage == Page::Home) {
        handleHomePageClick(mouseButton);
    }
}

void GUI::handleMouseMove(const sf::Event::MouseMoveEvent& mouseMove) {
    if (progressBar.getGlobalBounds().contains(mouseMove.x, mouseMove.y) && player.getStatus() == sf::SoundSource::Playing) {
        updateProgressBarPreview(mouseMove.x);
    }
    if (volumeSliderBackground.getGlobalBounds().contains(mouseMove.x, mouseMove.y)) {
        updateVolumeSliderPreview(mouseMove.x);
    }
}

void GUI::handleMouseRelease(const sf::Event::MouseButtonEvent& mouseButton) {
    if (progressBar.getGlobalBounds().contains(mouseButton.x, mouseButton.y) && player.getStatus() == sf::SoundSource::Playing) {
        setProgressFromMouseClick(mouseButton.x);
    }
}

void GUI::handleTextEntered(const sf::Event::TextEvent& text) {
    if (text.unicode == 8 && !searchQuery.empty()) { // Backspace
        searchQuery.pop_back();
    }
    else if (text.unicode < 128 && text.unicode != 8) {
        searchQuery += static_cast<char>(text.unicode);
    }
    searchText.setString(searchQuery);
    filterMusicFiles(player.getMusicFiles(), searchQuery, filteredMusicFiles);
}

void GUI::update() {
    if (player.hasStartedPlaying() && player.isCurrentSongFinished()) {
        player.next();
        playPauseButton.setTexture(pauseTexture);
        currentSong = getBaseName(player.getCurrentSong());
        clickedSongIndex = player.getCurrentIndex();
    }
    updateProgressBar();
    updateTimeDisplay();  // Add this line if it's not already there
}

void GUI::draw() {
    window.clear();

    window.draw(sidebar);
    for (const auto& text : sidebarTexts) {
        window.draw(text);
    }

    window.draw(contentArea);
    window.draw(controlBar);

    window.draw(playPauseButton);
    window.draw(nextButton);
    window.draw(prevButton);
    window.draw(shuffleButton);
    window.draw(loopButton);
    window.draw(volumeButton);

    window.draw(searchBar);
    window.draw(search);
    window.draw(searchText);

    if (isSearchBarActive) {
        drawSearchCursor();
    }

    window.draw(progressBar);
    window.draw(progressFill);

    if (clickedSongIndex != -1) {
        window.draw(currentTimeText);
        window.draw(totalTimeText);
    }
    window.draw(currentTimeText);
    window.draw(totalTimeText);

    window.draw(volumeSliderBackground);
    window.draw(volumeFill);

    switch (currentPage) {
    case Page::Home:
        drawHomePage();
        break;
    case Page::NowPlaying:
        drawNowPlayingPage();
        break;
    }

    window.display();
}

void GUI::drawHomePage() {
    const auto& displayFiles = searchQuery.empty() ? player.getMusicFiles() : filteredMusicFiles;
    for (size_t i = 0; i < displayFiles.size(); ++i) {
        sf::RectangleShape songButton(sf::Vector2f(window.getSize().x - 220.0f, 50.0f));
        songButton.setPosition(300.0f, 60.0f + i * 60.0f);
        songButton.setFillColor(sf::Color(70, 70, 70));

        // Find the index of this song in the original music files
 
       auto it = std::find(player.getMusicFiles().begin(), player.getMusicFiles().end(), displayFiles[i]);
        if (it != player.getMusicFiles().end()) {
            size_t originalIndex = std::distance(player.getMusicFiles().begin(), it);
            if (originalIndex == clickedSongIndex) {
                songButton.setFillColor(sf::Color::Red);
            }
        }

        window.draw(songButton);

        sf::Text songText;
        songText.setFont(font);
        songText.setString(getBaseName(displayFiles[i]));
        songText.setCharacterSize(30);
        songText.setFillColor(sf::Color::White);
        songText.setPosition(320.0f, 60.0f + i * 60.0f + 10.0f);
        window.draw(songText);

        size_t originalIndex = std::distance(player.getMusicFiles().begin(), it);
        if (originalIndex == clickedSongIndex && player.getStatus() == sf::SoundSource::Playing) {
            drawAnimationBars(songButton.getPosition());
        }
    }
}



void GUI::drawNowPlayingPage() {
    sf::Text songNameText;
    songNameText.setFont(font);
    std::string wrappedSongName = wrapText(currentSong, 30);
    songNameText.setString(wrappedSongName);
    songNameText.setCharacterSize(60);
    songNameText.setFillColor(sf::Color::White);
    songNameText.setPosition(contentArea.getPosition().x + (contentArea.getSize().x - songNameText.getLocalBounds().width) / 2,
        contentArea.getPosition().y + 50.0f);
    window.draw(songNameText);

    // Only update the animation time when the song is playing
    if (player.getStatus() == sf::SoundSource::Playing) {
        animationTime += clock.restart().asSeconds();
    } else {
        clock.restart(); // Restart the clock but don't update animationTime
    }

    // Determine which animation to show based on animationTime
    int animationIndex = static_cast<int>(animationTime / 10) % 3;

    switch (animationIndex) {
    case 0:
        drawBars();
        break;
    case 1:
        drawSpectrum();
        break;
    case 2:
        drawOscilloscope();
        break;
    }
}

void GUI::drawBars() {
    const int barCount = 30;
    const float barWidth = 20.0f;
    const float maxBarHeight = 200.0f;
    const float spacing = 5.0f;
    const float startX = contentArea.getPosition().x + (contentArea.getSize().x - (barCount * (barWidth + spacing) - spacing)) / 2;
    const float startY = contentArea.getPosition().y + contentArea.getSize().y / 2 + maxBarHeight / 2;

    for (int i = 0; i < barCount; ++i) {
        float height = (std::sin(animationTime * 5 + i * 0.5f) + 1) * maxBarHeight / 2;
        sf::RectangleShape bar(sf::Vector2f(barWidth, height));
        bar.setFillColor(sf::Color(29, 185, 84));
        bar.setPosition(startX + i * (barWidth + spacing), startY - height);
        window.draw(bar);
    }
}

void GUI::drawSpectrum() {
    const int pointCount = 1000;
    sf::VertexArray spectrum(sf::LineStrip, pointCount);
    const float width = contentArea.getSize().x;
    const float height = 200.0f;
    const float startX = contentArea.getPosition().x;
    const float startY = contentArea.getPosition().y + contentArea.getSize().y / 2;

    for (int i = 0; i < pointCount; ++i) {
        float x = startX + (static_cast<float>(i) / pointCount) * width;
        float y = startY - std::sin(x * 0.05f + animationTime * 5) * height / 2;
        spectrum[i].position = sf::Vector2f(x, y);
        spectrum[i].color = sf::Color(29, 185, 84);
    }

    // Draw the thick line
    sf::RectangleShape thickLine;
    thickLine.setFillColor(sf::Color(29, 185, 84));

    for (int i = 0; i < pointCount - 1; ++i) {
        sf::Vector2f point1 = spectrum[i].position;
        sf::Vector2f point2 = spectrum[i + 1].position;

        sf::Vector2f direction = point2 - point1;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        thickLine.setSize(sf::Vector2f(length, 5.0f));
        thickLine.setPosition(point1);
        thickLine.setRotation(std::atan2(direction.y, direction.x) * 180 / 3.14159f);

        window.draw(thickLine);
    }
}

void GUI::drawOscilloscope() {
    const int pointCount = 1000;
    const int lineCount = 10;
    const float lineSpacing = 2.0f;
    sf::VertexArray oscilloscope(sf::LineStrip, pointCount);
    const float width = contentArea.getSize().x;
    const float height = 200.0f;
    const float startX = contentArea.getPosition().x;
    const float startY = contentArea.getPosition().y + contentArea.getSize().y / 2;

    for (int line = 0; line < lineCount; ++line) {
        for (int i = 0; i < pointCount; ++i) {
            float x = startX + (static_cast<float>(i) / pointCount) * width;
            float y = startY + std::sin(x * 0.1f + animationTime * 10) * height / 4
                + std::cos(x * 0.05f + animationTime * 5) * height / 4;
            y += line * lineSpacing - (lineCount - 1) * lineSpacing / 2;
            oscilloscope[i].position = sf::Vector2f(x, y);
            oscilloscope[i].color = sf::Color(29, 185, 84);
        }
        window.draw(oscilloscope);
    }
}

void GUI::drawAnimationBars(const sf::Vector2f& position) {
    for (int i = 0; i < 3; ++i) {
        float height = (std::sin(clock.getElapsedTime().asSeconds() * 5 + i) + 1) * barMaxHeight / 2;
        animationBars[i].setSize(sf::Vector2f(5.0f, height));
        animationBars[i].setPosition(window.getSize().x - 50.0f + i * 10.0f, position.y + 25.0f - height / 2);
        window.draw(animationBars[i]);
    }
}

void GUI::drawSearchCursor() {
    if (cursorBlinkClock.getElapsedTime().asSeconds() < 0.5f) {
        sf::RectangleShape cursor(sf::Vector2f(2, 20));
        cursor.setFillColor(sf::Color::White);
        cursor.setPosition(15.0f + searchText.getLocalBounds().width, 15.0f);
        window.draw(cursor);
    }
    if (cursorBlinkClock.getElapsedTime().asSeconds() >= 1.0f) {
        cursorBlinkClock.restart();
    }
}

void GUI::togglePlayPause() {
    if (player.getStatus() == sf::SoundSource::Playing) {
        player.pause();
        playPauseButton.setTexture(playTexture);
    }
    else {
        player.play();
        playPauseButton.setTexture(pauseTexture);
    }
}

void GUI::toggleShuffle() {
    bool shuffleState = !player.getIsShuffled();
    player.shuffle(shuffleState);
    shuffleButton.setColor(shuffleState ? sf::Color::Green : sf::Color::White);
}

void GUI::toggleLoop() {
    bool loopState = !player.getIsLooping();
    player.loop(loopState);
    loopButton.setColor(loopState ? sf::Color::Green : sf::Color::White);
}

void GUI::setProgressFromMouseClick(float mouseX) {
    float clickPosition = (mouseX - progressBar.getPosition().x) / progressBar.getSize().x;
    clickPosition = std::clamp(clickPosition, 0.0f, 1.0f);
    
    float totalDuration = player.getTotalDuration();
    float newPosition = clickPosition * totalDuration;
    
    player.setPlaybackPosition(newPosition);
    
    // Update the progress bar visually
    updateProgressBar();
    
    // Update the time display
    updateTimeDisplay();
}


void GUI::setVolumeFromMouseClick(float mouseX) {
    float volume = (mouseX - volumeSliderBackground.getPosition().x) / volumeSliderBackground.getSize().x;
    volume = std::clamp(volume, 0.0f, 1.0f);
    volumeFill.setSize(sf::Vector2f(volume * volumeSliderBackground.getSize().x, volumeFill.getSize().y));
    player.setVolume(volume * 100);
}

void GUI::activateSearchBar() {
    isSearchBarActive = true;
    currentPage = Page::Home;
    search.setString(""); // Clear the search text when activating
    searchBar.setFillColor(sf::Color(80, 80, 80));
    cursorBlinkClock.restart();
}

void GUI::deactivateSearchBar() {
    // Check if there was any text in the search bar before setting the placeholder
    if (search.getString().isEmpty()) {
        search.setString("Search");
    }
    isSearchBarActive = false;
    searchBar.setFillColor(sf::Color(50, 50, 50));
}

void GUI::updateProgressBar() {
    float progress = player.getPlaybackPercentage();
    progressFill.setSize(sf::Vector2f(progress * progressBar.getSize().x, progressFill.getSize().y));
}

void GUI::updateProgressBarPreview(float mouseX) {
    float position = (mouseX - progressBar.getPosition().x) / progressBar.getSize().x;
    position = std::clamp(position, 0.0f, 1.0f);
    progressFill.setSize(sf::Vector2f(position * progressBar.getSize().x, progressFill.getSize().y));
}

void GUI::updateVolumeSliderPreview(float mouseX) {
    float volume = (mouseX - volumeSliderBackground.getPosition().x) / volumeSliderBackground.getSize().x;
    volume = std::clamp(volume, 0.0f, 1.0f);
    volumeFill.setSize(sf::Vector2f(volume * volumeSliderBackground.getSize().x, volumeFill.getSize().y));
    player.setVolume(volume * 100);
}

void GUI::handleHomePageClick(const sf::Event::MouseButtonEvent& mouseButton) {
    const auto& displayFiles = searchQuery.empty() ? player.getMusicFiles() : filteredMusicFiles;
    for (size_t i = 0; i < displayFiles.size(); ++i) {
        sf::RectangleShape songButton(sf::Vector2f(window.getSize().x - 220.0f, 50.0f));
        songButton.setPosition(300.0f, 60.0f + i * 60.0f);

        if (songButton.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
            // Find the index of the clicked song in the original music files
            auto it = std::find(player.getMusicFiles().begin(), player.getMusicFiles().end(), displayFiles[i]);
            if (it != player.getMusicFiles().end()) {
                size_t originalIndex = std::distance(player.getMusicFiles().begin(), it);

                if (originalIndex != player.getCurrentIndex() || player.getStatus() != sf::SoundSource::Playing) {
                    player.playSong(originalIndex);
                    player.play();
                    playPauseButton.setTexture(pauseTexture);
                }
                currentSong = getBaseName(displayFiles[i]);
                currentPage = Page::NowPlaying;
                clickedSongIndex = originalIndex;
                updateTimeDisplay();  // Update the time display immediately
                break;
            }
        }
    }
}