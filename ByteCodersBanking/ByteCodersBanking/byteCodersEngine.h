#ifndef BYTECODERSENGINE_H
#define BYTECODERSENGINE_H

#include <SFML/Graphics.hpp>
#include <string>


 class ByteCodersEngine {
    public:
        void main(const std::string& userEmail);

    private:
        float totalIncome = 0.0f;   // Member variable for total income
        float totalExpense = 0.0f;  // Member variable for total expenses
        float budget = 0.0f;        // Member variable for budget
 };


#endif // BYTECODERSENGINE_H
