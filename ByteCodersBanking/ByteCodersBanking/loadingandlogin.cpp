#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cstdlib> // For std::rand()
#include <iostream> // For debugging purposes

// Declare global variables for UI elements and state management
sf::RectangleShape textBox;
sf::RectangleShape loginButton;
sf::Text emailLabel;
sf::Text loginText;
sf::Text warningText; // Warning message
sf::Font font;
std::string emailInput;
bool emailInputActive = false;

// Variables for the login process
unsigned int windowWidth, windowHeight; // Variables for the window size
std::string UserID; // Variable for User ID
std::string LogInCode; // Variable for Login Code
bool loaded = false; // Variable to control the loading state
bool isLoginActive = false; // Flag to manage login state

sf::Clock buttonTimer;
bool isButtonPressed = false; // Flag to check if button was pressed
const float BUTTON_COLOR_CHANGE_DURATION = 0.5f; // Duration for which the button should be red

void handleInput(sf::Event event) {
    if (isLoginActive) {
        // Handle text input
        if (event.type == sf::Event::TextEntered) {
            if (emailInputActive) { // If the text box is active
                if (event.text.unicode < 128) { // Only accept ASCII input
                    if (event.text.unicode == '\b' && !emailInput.empty()) {
                        emailInput.pop_back(); // Handle backspace
                    }
                    else if (event.text.unicode != '\r') { // Ignore Enter key
                        emailInput += static_cast<char>(event.text.unicode); // Add character to input
                    }
                }
            }
        }

        // Handle mouse click events
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Check if the click is within the bounds of the text box
                emailInputActive = textBox.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

                // Check if the click is within the bounds of the login button
                if (loginButton.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y))) {
                    // Validate the email input
                    if ((emailInput.find("@gmail.com") != std::string::npos) ||
                        (emailInput.find("@abv.bg") != std::string::npos) ||
                        (emailInput.find("@outlook.com") != std::string::npos)) {

                        // Handle login button click
                        std::cout << "Email submitted: " << emailInput << std::endl; // For debugging purposes
                    }
                    else {
                        // Invalid email, turn the login button red
                        loginButton.setFillColor(sf::Color::Red);
                        isButtonPressed = true; // Set the flag that the button was pressed
                        buttonTimer.restart(); // Restart the timer
                    }
                }
            }
        }
    }
}

void loadAndResize() {
    // Create a borderless window with an initial size of 400x200 pixels
    sf::RenderWindow window(sf::VideoMode(400, 200), "Banking App", sf::Style::None);

    // Display the window for 5 seconds before resizing
    sf::Clock clock; // Create a clock to measure elapsed time
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black); // Clear the window
        window.display();

        // Wait for approximately 5 seconds
        if (clock.getElapsedTime().asSeconds() >= 5) {
            break; // Exit the loop after 5 seconds
        }
    }

    // Get the screen dimensions for resizing
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    windowWidth = static_cast<unsigned int>(desktopMode.width * 0.6); // Store the new width
    windowHeight = static_cast<unsigned int>(desktopMode.height * 0.6); // Store the new height

    // Change the window size, make it windowed, and set the title
    window.create(sf::VideoMode(windowWidth, windowHeight), "ByteCoders Loader", sf::Style::Titlebar | sf::Style::Close);

    // Load the background image
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("content/images/LogIn.png")) { // Update with your image path
        return; // Exit the function if loading fails
    }
    sf::Sprite backgroundSprite(backgroundTexture);

    // Scale the background to fit the window
    backgroundSprite.setScale(
        static_cast<float>(windowWidth) / backgroundTexture.getSize().x,
        static_cast<float>(windowHeight) / backgroundTexture.getSize().y
    );

    // Load the font
    if (!font.loadFromFile("content/fonts/SourceSansPro-Bold.otf")) { // Update with your font path
        return; // Exit the function if loading fails
    }

    // Prepare the warning message for email verification
    warningText.setString("Please make sure to write your correct email address so you can safely log in or you might trigger the automatic fraud detector");
    warningText.setFont(font);
    warningText.setCharacterSize(20); // Text size 20 for warning
    warningText.setFillColor(sf::Color::Red); // Set color to red

    // Generate the Login Code
    LogInCode = std::to_string(std::rand() % 100000); // Generate a random 5-digit code

    // Prepare the UI elements
    loginText.setString("LOG IN");
    loginText.setFont(font);
    loginText.setCharacterSize(50); // Text size 50
    loginText.setFillColor(sf::Color::White); // Set color to white
    loginText.setPosition(windowWidth / 2 - loginText.getGlobalBounds().width / 2, 50); // Centered

    // Text box for email input
    textBox.setSize(sf::Vector2f(300, 50)); // Width 300, Height 50
    textBox.setFillColor(sf::Color(255, 255, 255, 255)); // White background
    textBox.setPosition(windowWidth / 2 - textBox.getSize().x / 2, windowHeight / 2 - textBox.getSize().y / 2); // Centered

    // Button for login
    loginButton.setSize(sf::Vector2f(200, 50)); // Width 200, Height 50
    loginButton.setFillColor(sf::Color::Blue); // Blue button
    loginButton.setPosition(windowWidth / 2 - loginButton.getSize().x / 2, textBox.getPosition().y + textBox.getSize().y + 20); // Centered below text box

    // Label for email
    emailLabel.setString("Email");
    emailLabel.setFont(font);
    emailLabel.setCharacterSize(30); // Label size 30
    emailLabel.setFillColor(sf::Color::White); // Set label color to white
    emailLabel.setPosition(windowWidth / 2 - emailLabel.getGlobalBounds().width / 2, textBox.getPosition().y - 40); // Centered above text box

    // Position warning text below the login button
    warningText.setPosition(windowWidth / 2 - warningText.getGlobalBounds().width / 2, loginButton.getPosition().y + loginButton.getSize().y + 10); // Centered below the login button

    // Mark loaded as true
    loaded = true;

    // Start in login state
    isLoginActive = true;

    // Main event loop for the resized window
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            handleInput(event); // Call input handling function
        }

        // Check for hover effect on login button
        if (loginButton.getGlobalBounds().contains(static_cast<float>(sf::Mouse::getPosition(window).x), static_cast<float>(sf::Mouse::getPosition(window).y))) {
            if (!isButtonPressed) { // Only change color if button is not pressed
                loginButton.setFillColor(sf::Color::White); // Change button color on hover
            }
        }
        else {
            if (!isButtonPressed) {
                loginButton.setFillColor(sf::Color::Blue); // Reset button color
            }
        }

        // Update text box color when active
        if (emailInputActive) {
            textBox.setFillColor(sf::Color(134, 174, 230)); // Light blue when active
        }
        else {
            textBox.setFillColor(sf::Color(255, 255, 255)); // Reset to white
        }

        // Reset button color if it's been red for long enough
        if (isButtonPressed && buttonTimer.getElapsedTime().asSeconds() > BUTTON_COLOR_CHANGE_DURATION) {
            loginButton.setFillColor(sf::Color::Blue); // Reset to original color
            isButtonPressed = false; // Reset the flag
        }

        window.clear(); // Clear the window
        window.draw(backgroundSprite); // Draw the background
        window.draw(loginText); // Draw "LOG IN" text
        window.draw(emailLabel); // Draw the email label
        window.draw(textBox); // Draw the text box
        window.draw(loginButton); // Draw the login button
        window.draw(warningText); // Draw the warning message

        // Draw email input text inside the text box
        sf::Text inputText(emailInput, font, 30); // Input size 30
        inputText.setFillColor(sf::Color::Black); // Set input text color to black
        inputText.setPosition(textBox.getPosition().x + 10, textBox.getPosition().y + 10); // Position inside the text box
        window.draw(inputText); // Draw the input text

        // Draw "LOG IN" text on the button
        sf::Text buttonText("LOG IN", font, 30); // Button text size 30
        buttonText.setFillColor(sf::Color::White); // Set button text color to white
        buttonText.setPosition(loginButton.getPosition().x + loginButton.getSize().x / 2 - buttonText.getGlobalBounds().width / 2,
            loginButton.getPosition().y + loginButton.getSize().y / 2 - buttonText.getGlobalBounds().height / 2); // Centered on button
        window.draw(buttonText);

        window.display(); // Display everything drawn in the current frame
    }
}
