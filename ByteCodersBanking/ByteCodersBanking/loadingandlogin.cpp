// WELCOME RO loadingandlogin.cpp This random file handles everything related to loading,verification ect! Also it has imbeded python and I would say "NEVER AGAIN"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cstdlib> // For std::rand()
#include <iostream> // For debugging purposes
#include <ctime> // For time functions
#include <string>
#include <cctype> // For std::isalnum
#include "byteCodersEngine.h"
#include "Python/Python.h"

using namespace std;

// UI elements and state variables
sf::RectangleShape emailTextBox;
sf::RectangleShape codeTextBox; // New text box for code input
sf::RectangleShape loginButton;
sf::Text emailLabel;
sf::Text loginText;
sf::Text warningText; // Warning message
sf::Font font; // Font object
std::string emailInput;
std::string codeInput; // To hold the code input during verification
bool emailInputActive = false;
bool codeInputActive = false; // New flag for code input

// Login process variables
unsigned int windowWidth, windowHeight;
std::string LogInCode;
std::string emailAddress; // To store the email after initial submission
bool loaded = false;
bool isLoginActive = false;
bool isVerificationPhase = false;  // Flag to track login vs verification phase
sf::Clock verificationTimer;  // Timer for code expiration
bool isCodeExpired = false;    // Flag to check if code is expired
std::string UserID; // To store the user's email address permanently
sf::Clock buttonTimer;
bool isButtonPressed = false;
const float BUTTON_COLOR_CHANGE_DURATION = 0.5f;
bool verificationSuccess = false;

// Function to generate a random alphanumeric code
std::string generateRandomCode(int length) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string code;
    for (int i = 0; i < length; ++i) {
        code += alphanum[std::rand() % (sizeof(alphanum) - 1)]; // Exclude null terminator
    }
    return code;
}

void sendEmail(const string& recipient, const string& verificationCode) {
    Py_Initialize();
    // Load the module containing your 'send_email' function
    PyObject* pName = PyUnicode_DecodeFSDefault("email_sender");
    PyObject* pModule = PyImport_Import(pName);
    Py_XDECREF(pName);
    if (pModule != nullptr) {
        // Get the 'send_email' function
        PyObject* pFunc = PyObject_GetAttrString(pModule, "send_email");
        if (pFunc && PyCallable_Check(pFunc)) {
            PyObject* pArgs = PyTuple_Pack(2,
                PyUnicode_FromString(recipient.c_str()),
                PyUnicode_FromString(verificationCode.c_str()));
            if (pArgs != nullptr) {
                PyObject* pValue = PyObject_CallObject(pFunc, pArgs);
                Py_XDECREF(pArgs);
                if (pValue != nullptr) {
                    Py_XDECREF(pValue);
                    cout << "Email sent successfully!" << endl;
                }
                else {
                    PyErr_Print();
                    cerr << "Error sending email." << endl;
                }
            }
            else {
                cerr << "Error packing arguments for the Python function." << endl;
            }
        }
        else {
            if (pFunc == nullptr) {
                cerr << "Function send_email not found in module." << endl;
            }
            else {
                cerr << "send_email is not callable." << endl;
            }
            PyErr_Print();
        }
        Py_XDECREF(pFunc);
        Py_XDECREF(pModule);
    }
    else {
        cerr << "Failed to load the module." << endl;
        PyErr_Print();
    }
    Py_Finalize();
}

// Add a new flag
bool isEmailSubmitted = false;

void handleEmailInput(sf::Event event) {
    if (event.type == sf::Event::TextEntered) {
        if (emailInputActive) {  // Handle email input
            if (event.text.unicode < 128) {
                if (event.text.unicode == '\b') { // Handle backspace
                    if (!emailInput.empty()) { // Allow backspace
                        emailInput.pop_back();
                    }
                }
                else if (event.text.unicode != '\r') { // Ignore enter key
                    emailInput += static_cast<char>(event.text.unicode);
                }
            }
        }
    }
}

void handleCodeInput(sf::Event event) {
    if (event.type == sf::Event::TextEntered) {
        if (codeInputActive) { // Handle code input
            if (event.text.unicode < 128) {
                if (event.text.unicode == '\b') { // Handle backspace
                    if (!codeInput.empty()) {
                        codeInput.pop_back();
                    }
                }
                else if (event.text.unicode != '\r') { // Ignore enter key
                    codeInput += static_cast<char>(event.text.unicode);
                }
            }
        }
    }
}

void handleInput(sf::Event event) {
    if (isLoginActive) {
        // Call separate functions for email and code handling
        handleEmailInput(event);
        handleCodeInput(event);

        // Handle mouse input for interaction
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            // Check if user clicked the email input field
            emailInputActive = emailTextBox.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
            codeInputActive = codeTextBox.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

            // Check if user clicked the login button
            if (loginButton.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y))) {
                if (!isVerificationPhase) {
                    // Email submission phase
                    if ((emailInput.find("@gmail.com") != std::string::npos ||
                        emailInput.find("@abv.bg") != std::string::npos ||
                        emailInput.find("@outlook.com") != std::string::npos) &&
                        emailInput.length() > 5) { // Ensure email is longer than 5 characters

                        // Only set UserID if it's not already set
                        if (UserID.empty()) {
                            UserID = emailInput; // Only set once
                        }

                        std::cout << "Email submitted: " << UserID << std::endl; // Debug output

                        // Generate verification code
                        LogInCode = generateRandomCode(5);  // Generate a random 5-character code
                        std::cout << "Verification code sent to " << UserID << ": " << LogInCode << std::endl; // Debug output

                        // Send the email
                        sendEmail(emailInput, LogInCode);
                        // Prepare for verification phase
                        verificationTimer.restart();
                        isVerificationPhase = true;
                        emailInput.clear();    // Clear email input
                        codeInput.clear();      // Clear previous code input
                        emailInputActive = false;  // Deactivate email input for code entry
                        codeInputActive = true; // Activate code input for verification

                        // Update UI for verification phase
                        loginText.setString("VERIFY");
                        emailLabel.setString("Code");
                        loginButton.setFillColor(sf::Color::Green);
                        isCodeExpired = false; // Reset expired flag
                    }
                    else {
                        // Invalid email case
                        std::cout << "Invalid email format! Ensure it is longer than 5 characters." << std::endl; // Debug output
                        loginButton.setFillColor(sf::Color::Red);
                        isButtonPressed = true;
                        buttonTimer.restart();
                    }
                }
                else {
                    // Verification phase
                    std::cout << "Checking verification code..." << std::endl; // Debug output
                    std::cout << "Entered code: " << codeInput << ", Expected code: " << LogInCode << std::endl; // Debug output

                    if (codeInput == LogInCode) {
                        std::cout << "Verification successful!" << std::endl;
                        loginButton.setFillColor(sf::Color::Green);
                        // Pass the user's email to the ByteCodersEngine
                        ByteCodersEngine byteCodersEngine; // Create an instance of ByteCodersEngine
                        byteCodersEngine.main(UserID); // Pass the User email as a parameter
                    }
                    else {
                        // Incorrect code handling
                        std::cout << "Incorrect verification code entered!" << std::endl; // Debug output
                        loginButton.setFillColor(sf::Color::Red);
                        isButtonPressed = true;
                        buttonTimer.restart();
                    }
                }
            }
        }
    }
}

void loadAndResize() {
    // Initial window setup
    sf::RenderWindow window(sf::VideoMode(400, 200), "Banking App", sf::Style::None); // Set initial size to 200x400
    sf::Clock clock;

    // Load LoadingLogo texture for the small window
    sf::Texture loadingLogoTexture;
    if (!loadingLogoTexture.loadFromFile("content/images/LoadingLogo.png")) return; // Ensure the path is correct
    sf::Sprite loadingLogoSprite(loadingLogoTexture);
    loadingLogoSprite.setScale(
        static_cast<float>(400) / loadingLogoTexture.getSize().x,
        static_cast<float>(200) / loadingLogoTexture.getSize().y
    );

    // Display initial loading logo for 5 seconds
    while (window.isOpen() && clock.getElapsedTime().asSeconds() < 5) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;  // Exit the function
            }
        }
        window.clear();
        window.draw(loadingLogoSprite); // Draw loading logo
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

    // Setup UI elements
    warningText.setString("Please ensure you enter the correct details for safe login.");
    warningText.setFont(font);
    warningText.setCharacterSize(20);
    warningText.setFillColor(sf::Color::Red);
    warningText.setPosition(windowWidth / 2 - warningText.getGlobalBounds().width / 2, windowHeight - 50);

    loginText.setString("LOG IN");
    loginText.setFont(font);
    loginText.setCharacterSize(50);
    loginText.setFillColor(sf::Color::White);
    loginText.setPosition(windowWidth / 2 - loginText.getGlobalBounds().width / 2, 20);  // Adjusted position for visibility

    // Email text box
    emailTextBox.setSize(sf::Vector2f(300, 50));
    emailTextBox.setFillColor(sf::Color::White);
    emailTextBox.setPosition(windowWidth / 2 - emailTextBox.getSize().x / 2, windowHeight / 2 - emailTextBox.getSize().y / 2);

    // Code text box
    codeTextBox.setSize(sf::Vector2f(300, 50));
    codeTextBox.setFillColor(sf::Color::White);
    codeTextBox.setPosition(windowWidth / 2 - codeTextBox.getSize().x / 2, windowHeight / 2 - codeTextBox.getSize().y / 2);

    loginButton.setSize(sf::Vector2f(200, 50));
    loginButton.setFillColor(sf::Color::Blue);
    loginButton.setPosition(windowWidth / 2 - loginButton.getSize().x / 2, emailTextBox.getPosition().y + emailTextBox.getSize().y + 20);

    emailLabel.setString("Email");
    emailLabel.setFont(font);
    emailLabel.setCharacterSize(30);
    emailLabel.setFillColor(sf::Color::White);
    emailLabel.setPosition(windowWidth / 2 - emailLabel.getGlobalBounds().width / 2, emailTextBox.getPosition().y - 40);

    loaded = true;
    isLoginActive = true;

    // Define colors for text box states
    const sf::Color TEXT_BOX_IDLE_COLOR = sf::Color::White;
    const sf::Color TEXT_BOX_ACTIVE_COLOR = sf::Color(100, 100, 100); // Darker color for active state

    // In the main event loop, adjust the drawing of text boxes based on their active states
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return; // Exit the function after closing the window
            }
            handleInput(event);
        }

        // Clear the window and draw the background
        window.clear();
        window.draw(backgroundSprite);

        // Draw the appropriate input box based on the current state
        if (!isVerificationPhase) {
            // Set the color based on whether the email input is active
            emailTextBox.setFillColor(emailInputActive ? TEXT_BOX_ACTIVE_COLOR : TEXT_BOX_IDLE_COLOR);
            window.draw(emailTextBox); // Draw the email text box
            emailLabel.setString("Email");
            window.draw(emailLabel);
            // Draw the email input text
            sf::Text inputText(emailInput, font, 30);
            inputText.setFillColor(emailInputActive ? sf::Color::White : sf::Color::Black);
            inputText.setPosition(emailTextBox.getPosition().x + 10, emailTextBox.getPosition().y + 10);
            window.draw(inputText);
            // Draw warning text
            window.draw(warningText);
        }
        else {
            // Set the color based on whether the code input is active
            codeTextBox.setFillColor(codeInputActive ? TEXT_BOX_ACTIVE_COLOR : TEXT_BOX_IDLE_COLOR);
            window.draw(codeTextBox); // Draw the code text box
            emailLabel.setString("Code");
            window.draw(emailLabel);
            // Draw the code input text
            sf::Text codeInputText(codeInput, font, 30);
            codeInputText.setFillColor(codeInputActive ? sf::Color::White : sf::Color::Black);
            codeInputText.setPosition(codeTextBox.getPosition().x + 10, codeTextBox.getPosition().y + 10);
            window.draw(codeInputText);
        }

        // Hover effect for the login button
        if (loginButton.getGlobalBounds().contains(static_cast<float>(sf::Mouse::getPosition(window).x), static_cast<float>(sf::Mouse::getPosition(window).y))) {
            if (!isButtonPressed) loginButton.setFillColor(sf::Color::White);
        }
        else if (!isButtonPressed) {
            loginButton.setFillColor(sf::Color::Blue);
        }

        // Rendering login button
        sf::Text buttonText(isVerificationPhase ? "VERIFY" : "LOG IN", font, 30);
        buttonText.setFillColor(sf::Color::White);
        buttonText.setPosition(loginButton.getPosition().x + loginButton.getSize().x / 2 - buttonText.getGlobalBounds().width / 2,
            loginButton.getPosition().y + loginButton.getSize().y / 2 - buttonText.getGlobalBounds().height / 2);
        window.draw(loginButton);
        window.draw(buttonText);

        // Check if the verification code has expired after 60 seconds
        if (isVerificationPhase && verificationTimer.getElapsedTime().asSeconds() > 60) {
            std::cout << "Verification code has expired." << std::endl;
            isCodeExpired = true;  // Set expired flag
            isVerificationPhase = false;  // Reset back to login phase
            emailInput.clear();  // Clear the email input for re-entry
            codeInput.clear(); // Clear code input
            emailLabel.setString("Email");
            loginText.setString("LOG IN");
            loginButton.setFillColor(sf::Color::Blue);  // Reset button color
        }

        // Draw the LOG IN text at the top
        window.draw(loginText);

        window.display();
    }
}
