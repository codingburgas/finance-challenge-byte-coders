#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cstdlib> // For std::rand()
#include <iostream> // For debugging purposes
#include <ctime> // For time functions
#include <string>
#include <cctype> // For std::isalnum

// UI elements and state variables
sf::RectangleShape textBox;
sf::RectangleShape loginButton;
sf::Text emailLabel;
sf::Text loginText;
sf::Text warningText; // Warning message
sf::Font font;
std::string emailInput;
bool emailInputActive = false;

// Login process variables
unsigned int windowWidth, windowHeight;
std::string UserID;
std::string LogInCode;
std::string enteredCode;  // To hold the user-entered code in verification phase
std::string emailAddress; // To store the email after initial submission
bool loaded = false;
bool isLoginActive = false;
bool isVerificationPhase = false;  // Flag to track login vs verification phase
sf::Clock verificationTimer;  // Timer for code expiration
bool isCodeExpired = false;    // Flag to check if code is expired

sf::Clock buttonTimer;
bool isButtonPressed = false;
const float BUTTON_COLOR_CHANGE_DURATION = 0.5f;

// Function to generate a random alphanumeric code
std::string generateRandomCode(int length) {
    const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string code;

    for (int i = 0; i < length; ++i) {
        code += alphanum[std::rand() % (sizeof(alphanum) - 1)]; // Exclude null terminator
    }
    return code;
}

// Function to handle user input events
void handleInput(sf::Event event) {
    if (isLoginActive) {
        if (event.type == sf::Event::TextEntered && emailInputActive) {
            if (event.text.unicode < 128) { // ASCII only
                if (event.text.unicode == '\b' && !emailInput.empty()) {
                    emailInput.pop_back(); // Backspace handling
                }
                else if (event.text.unicode != '\r') {
                    emailInput += static_cast<char>(event.text.unicode); // Append valid character
                }
            }
        }
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            emailInputActive = textBox.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

            if (loginButton.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y))) {
                if (!isVerificationPhase) {
                    // Email submission phase
                    if (emailInput.find("@gmail.com") != std::string::npos ||
                        emailInput.find("@abv.bg") != std::string::npos ||
                        emailInput.find("@outlook.com") != std::string::npos) {
                        emailAddress = emailInput;  // Store the email address
                        std::cout << "Email submitted: " << emailInput << std::endl;

                        // Generate and send verification code (In a real app, you would send this via email)
                        LogInCode = generateRandomCode(5);  // Generate a random 5-character code
                        std::cout << "Verification code sent to " << emailAddress << ": " << LogInCode << std::endl;

                        // Start the verification timer
                        verificationTimer.restart();
                        isVerificationPhase = true;
                        emailInput.clear();  // Clear input for code entry

                        // Update UI for verification phase
                        loginText.setString("VERIFY");
                        emailLabel.setString("Code");
                        loginButton.setFillColor(sf::Color::Green);  // Indicate transition to verification
                        isCodeExpired = false; // Reset expired flag
                    }
                    else {
                        // Invalid email, turn the login button red
                        loginButton.setFillColor(sf::Color::Red);
                        isButtonPressed = true;
                        buttonTimer.restart();
                    }
                }
                else {
                    // Verification phase
                    if (emailInput == LogInCode) {
                        std::cout << "Verification successful!" << std::endl;
                        loginButton.setFillColor(sf::Color::Green);
                        // Transition to the next stage, if applicable
                    }
                    else {
                        loginButton.setFillColor(sf::Color::Red);  // Incorrect code
                        isButtonPressed = true;
                        buttonTimer.restart();
                        std::cout << "Incorrect verification code entered!" << std::endl;
                    }
                }
            }
        }
    }
}

// Loading and resizing function for the window and UI elements
void loadAndResize() {
    // Initial window setup
    sf::RenderWindow window(sf::VideoMode(400, 200), "Banking App", sf::Style::None);
    sf::Clock clock;

    // Display initial black screen for 5 seconds
    while (window.isOpen() && clock.getElapsedTime().asSeconds() < 5) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }
        window.clear(sf::Color::Black);
        window.display();
    }

    // Resizing window to 60% of the screen
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    windowWidth = static_cast<unsigned int>(desktopMode.width * 0.6);
    windowHeight = static_cast<unsigned int>(desktopMode.height * 0.6);
    window.create(sf::VideoMode(windowWidth, windowHeight), "ByteCoders Loader", sf::Style::Titlebar | sf::Style::Close);

    // Background image setup
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("content/images/LogIn.png")) return;
    sf::Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setScale(
        static_cast<float>(windowWidth) / backgroundTexture.getSize().x,
        static_cast<float>(windowHeight) / backgroundTexture.getSize().y
    );

    // Font loading
    if (!font.loadFromFile("content/fonts/SourceSansPro-Bold.otf")) return;

    // Warning message setup
    warningText.setString("Please ensure you enter the correct details for safe login.");
    warningText.setFont(font);
    warningText.setCharacterSize(20);
    warningText.setFillColor(sf::Color::Red);

    // Login text setup
    loginText.setString("LOG IN");
    loginText.setFont(font);
    loginText.setCharacterSize(50);
    loginText.setFillColor(sf::Color::White);
    loginText.setPosition(windowWidth / 2 - loginText.getGlobalBounds().width / 2, 50);

    // UI element properties
    textBox.setSize(sf::Vector2f(300, 50));
    textBox.setFillColor(sf::Color::White);
    textBox.setPosition(windowWidth / 2 - textBox.getSize().x / 2, windowHeight / 2 - textBox.getSize().y / 2);

    loginButton.setSize(sf::Vector2f(200, 50));
    loginButton.setFillColor(sf::Color::Blue);
    loginButton.setPosition(windowWidth / 2 - loginButton.getSize().x / 2, textBox.getPosition().y + textBox.getSize().y + 20);

    emailLabel.setString("Email");
    emailLabel.setFont(font);
    emailLabel.setCharacterSize(30);
    emailLabel.setFillColor(sf::Color::White);
    emailLabel.setPosition(windowWidth / 2 - emailLabel.getGlobalBounds().width / 2, textBox.getPosition().y - 40);

    warningText.setPosition(windowWidth / 2 - warningText.getGlobalBounds().width / 2, loginButton.getPosition().y + loginButton.getSize().y + 10);

    loaded = true;
    isLoginActive = true;

    // Main event loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            handleInput(event);
        }

        // Hover effect for the login button
        if (loginButton.getGlobalBounds().contains(static_cast<float>(sf::Mouse::getPosition(window).x), static_cast<float>(sf::Mouse::getPosition(window).y))) {
            if (!isButtonPressed) loginButton.setFillColor(sf::Color::White);
        }
        else if (!isButtonPressed) {
            loginButton.setFillColor(sf::Color::Blue);
        }

        // Check if the verification code has expired after 60 seconds
        if (isVerificationPhase && verificationTimer.getElapsedTime().asSeconds() > 60) {
            std::cout << "Verification code has expired." << std::endl;
            isCodeExpired = true;  // Set expired flag
            isVerificationPhase = false;  // Reset back to login phase
            emailInput.clear();  // Clear the email input for re-entry
            emailLabel.setString("Email");
            loginText.setString("LOG IN");
            loginButton.setFillColor(sf::Color::Blue);  // Reset button color
        }

        // Text box color when active
        textBox.setFillColor(emailInputActive ? sf::Color(134, 174, 230) : sf::Color::White);

        // Reset button color after specified duration
        if (isButtonPressed && buttonTimer.getElapsedTime().asSeconds() > BUTTON_COLOR_CHANGE_DURATION) {
            loginButton.setFillColor(sf::Color::Blue);
            isButtonPressed = false;
        }

        // Rendering
        window.clear();
        window.draw(backgroundSprite);
        window.draw(loginText);
        window.draw(emailLabel);
        window.draw(textBox);
        window.draw(loginButton);
        window.draw(warningText);

        // Display email or code input text
        sf::Text inputText(emailInput, font, 30);
        inputText.setFillColor(sf::Color::Black);
        inputText.setPosition(textBox.getPosition().x + 10, textBox.getPosition().y + 10);
        window.draw(inputText);

        // Draw "LOG IN" or "VERIFY" on the button
        sf::Text buttonText(isVerificationPhase ? "VERIFY" : "LOG IN", font, 30);
        buttonText.setFillColor(sf::Color::White);
        buttonText.setPosition(loginButton.getPosition().x + loginButton.getSize().x / 2 - buttonText.getGlobalBounds().width / 2,
            loginButton.getPosition().y + loginButton.getSize().y / 2 - buttonText.getGlobalBounds().height / 2);
        window.draw(buttonText);

        window.display();
    }
}