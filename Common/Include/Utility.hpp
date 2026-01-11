#pragma once

#include <string>

#include "nlohmann_json/json.hpp"

namespace IMChat
{
    std::string InputString(const char* text, const uint32_t minLength, const uint32_t maxLength, const char* restrictedSymbols);
    std::string SHA256(const std::string& str);
    nlohmann::json ParseJson(const std::vector<char>& data, const size_t size);
    std::string GetEnv(const char* name);
    void SleepUntil(const std::function<bool()>& predicate, const uint32_t timeoutMs);
    std::string GetTimeHhMm();
    size_t Utf8Strlen(const char* str);
}
