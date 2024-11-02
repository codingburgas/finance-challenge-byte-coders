// verify.cpp
#include "verify.h"
#include <iostream>
#include <cstdlib> // For std::rand()
#include <ctime>

// Initialize global variables for verification
bool isVerificationMode = false;
std::string verificationCode; // The correct code to verify
std::string userInputCode;    // User's input for verification

// Simulate sending a verification code by generating a random 5-digit code
void sendVerificationCode(const std::string& email) {
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed for random generator

    int code = std::rand() % 100000;         // Generate a random 5-digit code
    verificationCode = std::to_string(code); // Store it as the expected verification code

    // Simulate sending the code (for demonstration purposes)
    std::cout << "Verification code sent to " << email << ": " << verificationCode << std::endl;
}

// Check if the entered code matches the generated code
bool verifyCode(const std::string& inputCode) {
    return inputCode == verificationCode;
}

// Switch to verification mode and clear input for new code
void switchToVerificationMode() {
    isVerificationMode = true;
    userInputCode.clear(); // Clear previous input for verification
}
