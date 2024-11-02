#ifndef VERIFY_H
#define VERIFY_H

#include <string>

// Function declarations
void sendVerificationCode(const std::string& email);
bool verifyCode(const std::string& inputCode);
void switchToVerificationMode(); // Switches UI to verification mode

// Global variables to track verification state and input
extern bool isVerificationMode;    // Tracks if the user is in verification mode
extern std::string userInputCode;  // Stores the verification code input

#endif
