#include "byteCodersEngine.h"
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <stack>
#include <cctype>
#include <stdexcept>
#include "Python/Python.h"
#include <filesystem>

void openCalculatorWindow(sf::RenderWindow& window, const sf::Sprite& backgroundSprite, const sf::Text& emailText, bool& calculatorOpen, float& totalIncome, float& totalExpense, float& budget);

void openSaveStatisticsWindow(sf::RenderWindow& window, const sf::Sprite& backgroundSprite, const sf::Text& emailText, bool& saveStatisticsOpen, float totalIncome, float totalExpense, float budget);

void DownloadStats(const std::string& userEmail, double totalIncome, double totalExpense, double totalBudget) {
    // Determine if the budget is low or high
    std::string budgetStatus = (totalBudget < totalIncome - totalExpense) ? "Your budget is low." : "Your budget is high.";

    // Define the userData directory path
    std::string userDataDir = "userData";

    // Use the email as the filename
    std::string fileName = userEmail + "_stats.txt"; // Save with the email
    std::string filePath = userDataDir + "/" + fileName;

    // Create and open the text file at the specified path
    std::ofstream outputFile(filePath);

    if (outputFile.is_open()) {
        // Write the statistics to the file
        outputFile << "Email: " << userEmail << "\n";
        outputFile << "Income: " << totalIncome << "\n";
        outputFile << "Expense: " << totalExpense << "\n";
        outputFile << "Budget: " << totalBudget << "\n";
        outputFile << budgetStatus << "\n";

        outputFile.close(); // Close the file
        std::cout << "Statistics written to " << filePath << " successfully!" << std::endl;
    }
    else {
        std::cerr << "Failed to create the file." << std::endl;
    }

}




void ByteCodersEngine::main(const std::string& userEmail) {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    unsigned int windowWidth = static_cast<unsigned int>(desktopMode.width * 0.6);
    unsigned int windowHeight = static_cast<unsigned int>(desktopMode.height * 0.6);

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
    float budget = 0.0f;

    // Set up button textures
    std::vector<sf::Texture> buttonTextures(3); // Now we have 3 buttons
    const std::vector<std::string> buttonTexturePaths = {
        "content/images/accountButton1.png",  // Button 1
        "content/images/CalculatorButton2.png", // Button 2
        "content/images/SubscrionsButton4.png"  // Button 4
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

    // Set buttons in a horizontal row
    for (int i = 0; i < buttonTextures.size(); ++i) {
        buttons[i].setTexture(buttonTextures[i]);
        buttons[i].setPosition(
            (windowWidth - buttonWidth * 3 - spacing * 2) / 2 + i * (buttonWidth + spacing),
            windowHeight / 2 - (buttonHeight / 2) // Center vertically
        );
        buttons[i].setScale(
            static_cast<float>(buttonWidth) / buttonTextures[i].getSize().x,
            static_cast<float>(buttonHeight) / buttonTextures[i].getSize().y
        );
    }

    bool calculatorOpen = false;        // Flag to track calculator state
    bool saveStatisticsOpen = false;    // Flag to track Save Statistics state

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Check for mouse clicks and button bounds
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Check each button and open the corresponding screen
                if (buttons[1].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    calculatorOpen = true;      // Open Calculator screen
                    saveStatisticsOpen = false; // Close Save Statistics screen if open
                }
                if (buttons[2].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    saveStatisticsOpen = true;  // Open Save Statistics screen
                    calculatorOpen = false;     // Close Calculator screen if open
                }
            }
        }

        // Update hover effects
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for (size_t i = 0; i < buttons.size(); ++i) {
            if (buttons[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                if (!isHovered[i]) {
                    buttons[i].setScale(1.1f * static_cast<float>(buttonWidth) / buttonTextures[i].getSize().x,
                        1.1f * static_cast<float>(buttonHeight) / buttonTextures[i].getSize().y);
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

        // Clear window for drawing
        window.clear();
        window.draw(backgroundSprite);
        window.draw(emailText);

        // Draw the specific screen if one is open
        if (calculatorOpen) {
            openCalculatorWindow(window, backgroundSprite, emailText, calculatorOpen, totalIncome, totalExpense, budget);
        }
        else if (saveStatisticsOpen) {
            openSaveStatisticsWindow(window, backgroundSprite, emailText, saveStatisticsOpen, totalIncome, totalExpense, budget);
        }
        else {
            // If no screen is open, draw the buttons
            for (auto& button : buttons) {
                window.draw(button);
            }
        }

        // Display everything on the window
        window.display();
    }
}


void openSaveStatisticsWindow(sf::RenderWindow& window, const sf::Sprite& backgroundSprite, const sf::Text& emailText, bool& saveStatisticsOpen, float totalIncome, float totalExpense, float budget) {
    sf::Font font;
    if (!font.loadFromFile("content/fonts/SourceSansPro-Semibold.otf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return;
    }

    // Title for Save Statistics
    sf::Text titleText("SAVE STATISTICS", font, 30);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(window.getSize().x / 2 - titleText.getGlobalBounds().width / 2, 200);

    // Download button
    sf::RectangleShape downloadButton(sf::Vector2f(150, 50));
    downloadButton.setFillColor(sf::Color(50, 50, 150));
    downloadButton.setPosition(window.getSize().x / 2 - downloadButton.getSize().x / 2, 500);

    // Text on Download button
    sf::Text downloadText("Download", font, 24);
    downloadText.setFillColor(sf::Color::White);
    downloadText.setPosition(
        downloadButton.getPosition().x + (downloadButton.getSize().x - downloadText.getGlobalBounds().width) / 2,
        downloadButton.getPosition().y + (downloadButton.getSize().y - downloadText.getGlobalBounds().height) / 2
    );

    // Back button
    sf::RectangleShape backButton(sf::Vector2f(150, 50));
    backButton.setFillColor(sf::Color(200, 50, 50)); // Set the back button color to red
    backButton.setPosition(window.getSize().x / 2 - backButton.getSize().x / 2, 600);

    // Text on Back button
    sf::Text backText("Back", font, 24);
    backText.setFillColor(sf::Color::White);
    backText.setPosition(
        backButton.getPosition().x + (backButton.getSize().x - backText.getGlobalBounds().width) / 2,
        backButton.getPosition().y + (backButton.getSize().y - backText.getGlobalBounds().height) / 2
    );

    sf::Text messageText("Press Download to export a txt file of your statistics!", font, 20);
    messageText.setFillColor(sf::Color::White);
    messageText.setPosition(window.getSize().x / 2 - messageText.getGlobalBounds().width / 2, 700);

    bool downloadPressed = false;

    while (saveStatisticsOpen) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                saveStatisticsOpen = false;
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Handle download button
                if (downloadButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    downloadPressed = true;
                    messageText.setString("Download requested! Check in userData folder.");

                    // Extract the user email from the emailText object (assuming it's set correctly)
                    std::string userEmail = emailText.getString();

                    // Call the function to send the statistics via email
                    DownloadStats(userEmail, totalIncome, totalExpense, budget);
                }

                // Handle back button
                if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    saveStatisticsOpen = false; // Close the statistics window to return to the main menu
                }
            }
        }

        // Hover effect for buttons
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Download button hover effect
        if (downloadButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            downloadButton.setSize(sf::Vector2f(155, 55)); // Slightly enlarge the button
            downloadButton.setFillColor(sf::Color(70, 70, 200)); // Darken color
        }
        else {
            downloadButton.setSize(sf::Vector2f(150, 50)); // Reset size
            downloadButton.setFillColor(sf::Color(50, 50, 150)); // Reset color
        }

        // Back button hover effect
        if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            backButton.setSize(sf::Vector2f(155, 55)); // Slightly enlarge the button
            backButton.setFillColor(sf::Color(220, 70, 70)); // Darken red color
        }
        else {
            backButton.setSize(sf::Vector2f(150, 50)); // Reset size
            backButton.setFillColor(sf::Color(200, 50, 50)); // Reset color
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(emailText);
        window.draw(titleText);
        window.draw(downloadButton);
        window.draw(downloadText);
        window.draw(backButton);
        window.draw(backText);
        window.draw(messageText);
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

    // Initialize button properties for calculator
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
            window.getSize().x / 2 - (buttonWidth * 2 + buttonSpacing) + (i % 5) * (buttonWidth + buttonSpacing) - 50,
            400 + (i / 5) * (buttonHeight + buttonSpacing)
        );
        button.setFillColor(sf::Color(50, 50, 50)); // Set a background color for buttons
        buttons.push_back(button);
    }

    // Create button text objects for calculator buttons
    std::vector<sf::Text> buttonTexts;
    for (const auto& label : buttonLabels) {
        sf::Text buttonText;
        buttonText.setFont(font);
        buttonText.setString(label);
        buttonText.setCharacterSize(24);
        buttonText.setFillColor(sf::Color::White);
        buttonTexts.push_back(buttonText);
    }

    // Position button texts for calculator buttons
    for (size_t i = 0; i < buttonTexts.size(); ++i) {
        buttonTexts[i].setPosition(
            buttons[i].getPosition().x + (buttonWidth / 2) - (buttonTexts[i].getGlobalBounds().width / 2),
            buttons[i].getPosition().y + (buttonHeight / 2) - (buttonTexts[i].getGlobalBounds().height / 2)
        );
    }

    // Create back button
    sf::RectangleShape backButton(sf::Vector2f(100, 50));
    backButton.setFillColor(sf::Color(200, 10, 10)); // Set a background color for back button
    backButton.setPosition(window.getSize().x - 110, 150); // Position in the upper right corner with some margin

    sf::Text backButtonText;
    backButtonText.setFont(font);
    backButtonText.setString("Back");
    backButtonText.setCharacterSize(24);
    backButtonText.setFillColor(sf::Color::White);
    backButtonText.setPosition(
        backButton.getPosition().x + (backButton.getSize().x / 2) - (backButtonText.getGlobalBounds().width / 2),
        backButton.getPosition().y + (backButton.getSize().y / 2) - (backButtonText.getGlobalBounds().height / 2)
    );

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

                // Check if back button is pressed
                if (backButton.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y))) {
                    calculatorOpen = false; // Close the calculator and return to the main menu
                }
            }
        }

        // Get the current mouse position
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Hover effect for calculator buttons
        for (size_t i = 0; i < buttons.size(); ++i) {
            if (buttons[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                buttons[i].setSize(sf::Vector2f(buttonWidth * 1.1f, buttonHeight * 1.1f)); // Increase size
                buttons[i].setFillColor(sf::Color(70, 70, 70)); // Darker color
            }
            else {
                buttons[i].setSize(sf::Vector2f(buttonWidth, buttonHeight)); // Reset to original size
                buttons[i].setFillColor(sf::Color(50, 50, 50)); // Reset to original color
            }
        }

        // Hover effect for back button
        if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            backButton.setSize(sf::Vector2f(110, 60)); // Increase size
            backButton.setFillColor(sf::Color(105, 0, 0)); // Darker color
        }
        else {
            backButton.setSize(sf::Vector2f(100, 50)); // Reset to original size
            backButton.setFillColor(sf::Color(255, 50, 50)); // Reset to original color
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

        // Draw calculator buttons and their text
        for (size_t i = 0; i < buttons.size(); ++i) {
            window.draw(buttons[i]);
            window.draw(buttonTexts[i]);
        }

        // Draw the back button and its text
        window.draw(backButton);
        window.draw(backButtonText);

        // Display the contents of the window
        window.display();
    }
}