#include "Utility.hpp"

#include <iostream>

namespace IMChat
{
    std::string InputString()
    {
        std::string input;
        while (true)
        {
            if (std::getline(std::cin >> std::ws, input) && !input.empty())
                return input;

            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Please reenter: ";
        }
    }
}
