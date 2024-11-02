#include "byteCodersEngine.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <stack>
#include <cctype>
#include <stdexcept>

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

    // Local variables for financial data
    float totalIncome = 0.0f;
    float totalExpense = 0.0f;

    // Set up button textures
    std::vector<sf::Texture> buttonTextures(5);
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
    std::vector<sf::Sprite> buttons(buttonTextures.size());
    std::vector<bool> isHovered(buttonTextures.size(), false);

    // Set buttons in a 2x3 grid
    for (int i = 0; i < buttonTextures.size(); ++i) {
        buttons[i].setTexture(buttonTextures[i]);
        buttons[i].setPosition(
            (windowWidth - buttonWidth * 3 - spacing * 2) / 2 + (i % 3) * (buttonWidth + spacing),
            windowHeight / 2 - (buttonHeight * 1) / 2 + (i / 3) * (buttonHeight + spacing) // Center vertically
        );
        buttons[i].setScale(
            static_cast<float>(buttonWidth) / buttonTextures[i].getSize().x,
            static_cast<float>(buttonHeight) / buttonTextures[i].getSize().y
        );
    }

    bool calculatorOpen = false; // Flag to track calculator state

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
            openCalculatorWindow(window, backgroundSprite, emailText, calculatorOpen, totalIncome, totalExpense,budget);
        }

        window.display();
    }
}

float calculateExpression(const std::string& expression) {
    std::istringstream iss(expression);
    std::stack<float> values;   // Stack to hold values
    std::stack<char> ops;       // Stack to hold operators
    float value;
    char op;

    // Helper function to apply an operator to the top two values in the stack
    auto applyOperation = [](float a, float b, char operation) {
        switch (operation) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) throw std::runtime_error("Division by zero");
            return a / b;
        default: throw std::runtime_error("Unknown operator");
        }
        };

    // Read the expression
    while (iss >> value) {
        values.push(value); // Push value to the value stack
        if (iss >> op) {    // Check for the next operator
            // Handle precedence for * and /
            while (!ops.empty() && (ops.top() == '*' || ops.top() == '/') && (op == '+' || op == '-')) {
                float b = values.top(); values.pop();
                float a = values.top(); values.pop();
                char operation = ops.top(); ops.pop();
                values.push(applyOperation(a, b, operation));
            }
            ops.push(op); // Push the operator to the operator stack
        }
    }

    // Apply remaining operations in the stacks
    while (!ops.empty()) {
        float b = values.top(); values.pop();
        float a = values.top(); values.pop();
        char operation = ops.top(); ops.pop();
        values.push(applyOperation(a, b, operation));
    }

    // The result will be the last remaining value on the values stack
    return values.top();
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
    display.setPosition(window.getSize().x / 2 - 190, 300); // Center the display horizontally

    sf::Text displayText;
    displayText.setFont(font);
    displayText.setCharacterSize(24);
    displayText.setFillColor(sf::Color::White);
    displayText.setPosition(display.getPosition().x + 15, display.getPosition().y + 5); // Position inside the display rectangle

    std::string currentExpression;

    // Create text objects for income, expense, and budget
    sf::Text incomeText, expenseText, budgetText;
    incomeText.setFont(font);
    expenseText.setFont(font);
    budgetText.setFont(font);
    incomeText.setCharacterSize(24);
    expenseText.setCharacterSize(24);
    budgetText.setCharacterSize(24);
    incomeText.setFillColor(sf::Color::White);
    expenseText.setFillColor(sf::Color::White);
    budgetText.setFillColor(sf::Color::White);
    incomeText.setPosition(20, 100);
    expenseText.setPosition(20, 140);
    budgetText.setPosition(20, 180);

    // Initialize button properties
    std::vector<std::string> buttonLabels = { "7", "8", "9", "/", "C",
                                               "4", "5", "6", "*", "=",
                                               "1", "2", "3", "-", "0",
                                               "+", "I", "E" };

    std::vector<sf::RectangleShape> buttons;
    int buttonWidth = 60;
    int buttonHeight = 60;
    int buttonSpacing = 10;

    // Create buttons for the calculator
    for (int i = 0; i < buttonLabels.size(); ++i) {
        sf::RectangleShape button(sf::Vector2f(buttonWidth, buttonHeight));
        button.setPosition(
            window.getSize().x / 2 - (buttonWidth * 2 + buttonSpacing) + (i % 5) * (buttonWidth + buttonSpacing)-50,
            400 + (i / 5) * (buttonHeight + buttonSpacing)
        );
        button.setFillColor(sf::Color(50, 50, 50)); // Set a background color for buttons
        buttons.push_back(button);
    }

    // Create button text objects
    std::vector<sf::Text> buttonTexts;
    for (const auto& label : buttonLabels) {
        sf::Text buttonText;
        buttonText.setFont(font);
        buttonText.setString(label);
        buttonText.setCharacterSize(24);
        buttonText.setFillColor(sf::Color::White);
        buttonTexts.push_back(buttonText);
    }

    // Position button texts
    for (size_t i = 0; i < buttonTexts.size(); ++i) {
        buttonTexts[i].setPosition(
            buttons[i].getPosition().x + (buttonWidth / 2) - (buttonTexts[i].getGlobalBounds().width / 2),
            buttons[i].getPosition().y + (buttonHeight / 2) - (buttonTexts[i].getGlobalBounds().height / 2)
        );
    }

    while (calculatorOpen) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                calculatorOpen = false;
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                for (size_t i = 0; i < buttons.size(); ++i) {
                    if (buttons[i].getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y))) {
                        if (buttonLabels[i] == "C") {
                            currentExpression.clear(); // Clear the current expression
                        }
                        else if (buttonLabels[i] == "=") {
                            // Calculate if the expression is not empty
                            if (!currentExpression.empty()) {
                                try {
                                    float result = calculateExpression(currentExpression);
                                    currentExpression = std::to_string(result);
                                }
                                catch (const std::runtime_error& e) {
                                    currentExpression = "0"; // Display error message
                                }
                            }
                        }
                        else if (buttonLabels[i] == "I") {
                            // Add income
                            if (!currentExpression.empty()) {
                                totalIncome += std::stof(currentExpression);
                                budget = totalIncome - totalExpense; // Update budget automatically
                                currentExpression.clear();
                            }
                        }
                        else if (buttonLabels[i] == "E") {
                            // Add expense
                            if (!currentExpression.empty()) {
                                totalExpense += std::stof(currentExpression);
                                budget = totalIncome - totalExpense; // Update budget automatically
                                currentExpression.clear();
                            }
                        }
                        else {
                            // Append the button label to the current expression
                            currentExpression += buttonLabels[i];
                        }
                    }
                }
            }
        }

        // Update the display text and other texts
        displayText.setString(currentExpression);
        incomeText.setString("Income: " + std::to_string(totalIncome));
        expenseText.setString("Expense: " + std::to_string(totalExpense));
        budgetText.setString("Budget: " + std::to_string(budget));

        window.clear();
        window.draw(backgroundSprite);
        window.draw(emailText);
        window.draw(display);
        window.draw(displayText);
        window.draw(incomeText);
        window.draw(expenseText);
        window.draw(budgetText);

        // Draw buttons and their text
        for (size_t i = 0; i < buttons.size(); ++i) {
            window.draw(buttons[i]);
            window.draw(buttonTexts[i]);
        }

        window.display();
    }
}

