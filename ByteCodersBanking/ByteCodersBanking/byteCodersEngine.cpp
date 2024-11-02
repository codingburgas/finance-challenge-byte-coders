#include "byteCodersEngine.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

void ByteCodersEngine::main(const std::string& userEmail) { // Accept email as a parameter
    // Retrieve desktop mode and set window size to 60% of the screen dimensions
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    unsigned int windowWidth = static_cast<unsigned int>(desktopMode.width * 0.6);
    unsigned int windowHeight = static_cast<unsigned int>(desktopMode.height * 0.6);
    std::cout << userEmail;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "ByteCoders Engine");

    // Load background
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("content/images/ByteCodersMainMenu.png")) {
        std::cerr << "Failed to load background image!" << std::endl;
        return;
    }

    // Set up the background sprite to scale with the window size
    sf::Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setScale(
        static_cast<float>(windowWidth) / backgroundTexture.getSize().x,
        static_cast<float>(windowHeight) / backgroundTexture.getSize().y
    );

    // Load font (ensure the path is correct)
    sf::Font font;
    if (!font.loadFromFile("content/fonts/SourceSansPro-Semibold.otf")) { // Replace with your font file
        std::cerr << "Failed to load font!" << std::endl;
        return;
    }

    // Create email text
    sf::Text emailText;
    emailText.setFont(font);
    emailText.setString(userEmail);
    emailText.setCharacterSize(30); // Set size for the email text
    emailText.setFillColor(sf::Color::White); // Set color for the email text
    emailText.setPosition(windowWidth / 2 - emailText.getGlobalBounds().width / 2, 50); // Center it above the buttons

    // Set up button textures (ensure the paths are correct)
    std::vector<sf::Texture> buttonTextures(6);
    const std::vector<std::string> buttonTexturePaths = {
        "content/images/accountButton1.png",
        "content/images/CalculatorButton2.png",
        "content/images/transactionButton3.png",
        "content/images/SubscrionsButton4.png",
        "content/images/BudjetButton5.png",
        "content/images/CalculatorButton2.png"
    };

    // Load button textures
    for (size_t i = 0; i < buttonTextures.size(); ++i) {
        if (!buttonTextures[i].loadFromFile(buttonTexturePaths[i])) {
            std::cerr << "Failed to load button texture: " << buttonTexturePaths[i] << std::endl;
            return;
        }
    }

    // Set up buttons in a 3x2 grid (3 columns, 2 rows), centered horizontally
    const int buttonWidth = 100;
    const int buttonHeight = 100;
    const int spacing = 30;

    std::vector<sf::Sprite> buttons(6);
    for (int i = 0; i < 6; ++i) {
        buttons[i].setTexture(buttonTextures[i]);
        buttons[i].setPosition(
            windowWidth / 2 - (3 * (buttonWidth + spacing)) / 2 + (i % 3) * (buttonWidth + spacing),
            windowHeight / 2 - (2 * (buttonHeight + spacing)) / 2 + (i / 3) * (buttonHeight + spacing)
        );
        buttons[i].setScale(
            static_cast<float>(buttonWidth) / buttonTextures[i].getSize().x,
            static_cast<float>(buttonHeight) / buttonTextures[i].getSize().y
        );
    }

    // Render loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Draw background, email text, and buttons
        window.clear();
        window.draw(backgroundSprite);
        window.draw(emailText); // Draw the email text above the buttons
        for (auto& button : buttons) {
            window.draw(button);
        }
        window.display();
    }
}
