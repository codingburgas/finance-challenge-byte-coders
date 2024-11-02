#include "byteCodersEngine.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>

// Function prototype
void openCalculatorWindow(sf::RenderWindow& window, const sf::Sprite& backgroundSprite, const sf::Text& emailText, bool& calculatorOpen, float& totalIncome, float& totalExpense, float& budget);

void ByteCodersEngine::main(const std::string& userEmail) {
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

    sf::Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setScale(
        static_cast<float>(windowWidth) / backgroundTexture.getSize().x,
        static_cast<float>(windowHeight) / backgroundTexture.getSize().y
    );

    // Load font
    sf::Font font;
    if (!font.loadFromFile("content/fonts/SourceSansPro-Semibold.otf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return;
    }

    // Create email text
    sf::Text emailText;
    emailText.setFont(font);
    emailText.setString(userEmail);
    emailText.setCharacterSize(30);
    emailText.setFillColor(sf::Color::White);
    emailText.setPosition(windowWidth / 2 - emailText.getGlobalBounds().width / 2, 50);

    // Set up button textures
    std::vector<sf::Texture> buttonTextures(5); // Adjusted to 5 buttons
    const std::vector<std::string> buttonTexturePaths = {
        "content/images/accountButton1.png",
        "content/images/CalculatorButton2.png",
        "content/images/transactionButton3.png",
        "content/images/SubscrionsButton4.png",
        "content/images/BudjetButton5.png"
    };

    for (size_t i = 0; i < buttonTextures.size(); ++i) {
        if (!buttonTextures[i].loadFromFile(buttonTexturePaths[i])) {
            std::cerr << "Failed to load button texture: " << buttonTexturePaths[i] << std::endl;
            return;
        }
    }

    const int buttonWidth = 100;
    const int buttonHeight = 100;
    const int spacing = 30;
    const sf::Color hoverColor = sf::Color(128, 0, 128, 200);

    std::vector<sf::Sprite> buttons(buttonTextures.size());
    std::vector<bool> isHovered(buttonTextures.size(), false);

    for (int i = 0; i < buttonTextures.size(); ++i) {
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

    bool calculatorOpen = false; // Flag to track calculator state
    float totalIncome = 0.0f;
    float totalExpense = 0.0f;
    float budget = 0.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (!calculatorOpen && buttons[1].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    calculatorOpen = true; // Open the calculator
                }
            }
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Check for hover effects
        for (int i = 0; i < buttons.size(); ++i) {
            if (!calculatorOpen) {
                if (buttons[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (!isHovered[i]) {
                        buttons[i].setScale(1.2f * buttons[i].getScale().x, 1.2f * buttons[i].getScale().y);
                        isHovered[i] = true;
                    }
                }
                else {
                    if (isHovered[i]) {
                        buttons[i].setScale(static_cast<float>(buttonWidth) / buttonTextures[i].getSize().x,
                            static_cast<float>(buttonHeight) / buttonTextures[i].getSize().y);
                        isHovered[i] = false;
                    }
                }
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(emailText);

        if (!calculatorOpen) {
            for (auto& button : buttons) {
                window.draw(button);
            }
        }
        else {
            openCalculatorWindow(window, backgroundSprite, emailText, calculatorOpen, totalIncome, totalExpense, budget);
        }

        window.display();
    }
}

float calculateExpression(const std::string& expression) {
    std::istringstream iss(expression);
    float result;
    iss >> result; // Simple conversion, needs actual expression parsing
    return result;
}

void openCalculatorWindow(sf::RenderWindow& window, const sf::Sprite& backgroundSprite, const sf::Text& emailText, bool& calculatorOpen, float& totalIncome, float& totalExpense, float& budget) {
    // Load font for button text
    sf::Font font;
    if (!font.loadFromFile("content/fonts/SourceSansPro-Semibold.otf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return;
    }

    // Display area for typed input
    sf::RectangleShape display(sf::Vector2f(380, 50));
    display.setFillColor(sf::Color::Black);
    display.setPosition(10, 20);

    sf::Text displayText;
    displayText.setFont(font);
    displayText.setCharacterSize(24);
    displayText.setFillColor(sf::Color::White);
    displayText.setPosition(15, 25);

    std::string currentExpression;

    // Income, Expense, and Budget text
    sf::Text incomeText, expenseText, budgetText;
    incomeText.setFont(font);
    incomeText.setCharacterSize(24);
    incomeText.setFillColor(sf::Color::Green);
    incomeText.setPosition(10, 100); // Adjusted position

    expenseText.setFont(font);
    expenseText.setCharacterSize(24);
    expenseText.setFillColor(sf::Color::Red);
    expenseText.setPosition(10, 150); // Adjusted position

    budgetText.setFont(font);
    budgetText.setCharacterSize(24);
    budgetText.setFillColor(sf::Color::White);
    budgetText.setPosition(10, 200); // Adjusted position

    // Define buttons for numbers and operations
    std::vector<sf::Text> buttons;
    std::string buttonLabels = "1234567890+-*/=DIE"; // 'D' for Delete, 'I' for Income, 'E' for Expense
    int buttonSize = 50;
    int padding = 10;
    int xStart = (window.getSize().x - 4 * (buttonSize + padding)) / 2; // Centered
    int yStart = (window.getSize().y - 5 * (buttonSize + padding)) / 2; // Centered

    // Back button texture
    sf::Texture backButtonTexture;
    if (!backButtonTexture.loadFromFile("content/images/back.png")) {
        std::cerr << "Failed to load back button texture!" << std::endl;
        return;
    }
    sf::Sprite backButton(backButtonTexture);
    backButton.setPosition(window.getSize().x - 80, 10); // Position the back button

    for (size_t i = 0; i < buttonLabels.size(); ++i) {
        sf::Text button;
        button.setFont(font);
        button.setCharacterSize(24);
        button.setFillColor(sf::Color::White);
        button.setString(std::string(1, buttonLabels[i]));
        button.setPosition(xStart + (i % 4) * (buttonSize + padding), yStart + (i / 4) * (buttonSize + padding));
        buttons.push_back(button);
    }

    while (calculatorOpen) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                calculatorOpen = false;
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    calculatorOpen = false; // Close the calculator
                }

                for (size_t i = 0; i < buttons.size(); ++i) {
                    if (buttons[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        char pressedChar = buttonLabels[i];
                        if (pressedChar == '=') {
                            totalIncome = calculateExpression(currentExpression); // Here you would implement a proper expression evaluation
                            incomeText.setString("Total Income: " + std::to_string(totalIncome));
                            currentExpression.clear(); // Clear expression after calculation
                        }
                        else if (pressedChar == 'D') {
                            currentExpression.clear(); // Clear the current expression
                        }
                        else if (pressedChar == 'I') {
                            totalIncome += calculateExpression(currentExpression);
                            incomeText.setString("Total Income: " + std::to_string(totalIncome));
                            currentExpression.clear(); // Clear expression after adding
                        }
                        else if (pressedChar == 'E') {
                            totalExpense += calculateExpression(currentExpression);
                            expenseText.setString("Total Expense: " + std::to_string(totalExpense));
                            currentExpression.clear(); // Clear expression after adding
                        }
                        else {
                            currentExpression += pressedChar; // Add the pressed character to the expression
                        }
                        displayText.setString(currentExpression);
                    }
                }
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(emailText);
        window.draw(display);
        window.draw(displayText);
        window.draw(incomeText);
        window.draw(expenseText);
        window.draw(budgetText);
        window.draw(backButton);

        for (const auto& button : buttons) {
            window.draw(button);
        }

        window.display();
    }
}
