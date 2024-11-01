#include <SFML/Graphics.hpp>  // For SFML functionalities
#include <iostream>  // For SFML functionalities
// Declare the function to load and resize from loading.cpp
void loadAndResize(); // Updated to not pass the window

int main() {
    // Call loadAndResize to create the window and update settings
    loadAndResize();

    return 0; // Exit the program
}
