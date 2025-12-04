#include "Utility.hpp"

#include <print>
#include <iostream>
#include <unordered_set>

#include "picosha2/picosha2.h"

namespace IMChat
{
    std::string InputString(const char* text, const uint32_t minLength, const uint32_t maxLength, const char* restrictedSymbols)
    {
        std::unordered_set<char> restrictedSymbolsSet;

        const auto rSLength = strlen(restrictedSymbols);
        for (uint32_t i = 0; i < rSLength; i++)
            restrictedSymbolsSet.insert(restrictedSymbols[i]);

        std::string input;

        while (true)
        {
            std::cin.clear();
            std::print("{}", text);

            if (!std::getline(std::cin >> std::ws, input))
            {
                std::println("Invalid input!");
                continue;
            }

            if (input.length() < minLength || input.length() > maxLength)
            {
                std::println("Input length {} is outside of the allowed range of [{}, {}]!", input.length(), minLength, maxLength);
                continue;
            }

            bool restrictedFound = false;
            for (const auto c : input)
            {
                if (restrictedSymbolsSet.contains(c))
                {
                    std::println("Character '{}' is not allowed!", c);
                    restrictedFound = true;
                    break;
                }
            }

            if (restrictedFound)
                continue;

            return input;
        }
    }

    std::string SHA256(const std::string& str)
    {
        return picosha2::hash256_hex_string(str);
    }

    nlohmann::json ParseJson(const std::vector<char>& data, const size_t size)
    {
        std::string jsonStr(data.data(), size);
        return nlohmann::json::parse(jsonStr);
    }
}
