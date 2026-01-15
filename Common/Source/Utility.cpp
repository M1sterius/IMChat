#include "Utility.hpp"

#include <print>
#include <chrono>
#include <format>
#include <iostream>
#include <thread>
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

    std::string GetEnv(const char* name)
    {
        const auto var = std::getenv(name);
        return std::string(var ? var : "");
    }

    void SleepUntil(const std::function<bool()>& predicate, const uint32_t timeoutMs)
    {
        using namespace std::chrono_literals;

        const auto start = std::chrono::high_resolution_clock::now();
        while (predicate())
        {
            const auto end = std::chrono::high_resolution_clock::now();
            const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            if (elapsedMs.count() > timeoutMs)
                break;

            std::this_thread::sleep_for(100ms);
        }
    }

    std::string GetTimeHhMm()
    {
        const auto now = std::chrono::system_clock::now();
        const auto t   = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};

    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif

        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M");
        return oss.str();
    }

    size_t Utf8Strlen(const char* str)
    {
        size_t count = 0;

        while (*str != 0)
        {
            if ((*str & 0xc0) != 0x80)
                count++;
            str++;
        }

        return count;
    }

    std::string TimestampZ()
    {
        const auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::tm tm_utc;
        #ifdef _WIN32
        gmtime_s(&tm_utc, &time_t_now);
        #else
        gmtime_r(&time_t_now, &tm_utc);
        #endif

        std::ostringstream oss;
        oss << std::put_time(&tm_utc, "%Y-%m-%d %H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        oss << "+00";

        return oss.str();
    }

    ParsedTimestamp ParseTimestamp(const std::string& timestampz)
    {
        ParsedTimestamp result;

        std::tm tm_utc = {};
        std::istringstream iss(timestampz);

        char delimiter;
        iss >> tm_utc.tm_year >> delimiter >> tm_utc.tm_mon >> delimiter >> tm_utc.tm_mday;
        iss >> tm_utc.tm_hour >> delimiter >> tm_utc.tm_min >> delimiter >> tm_utc.tm_sec;

        tm_utc.tm_year -= 1900;
        tm_utc.tm_mon -= 1;
        tm_utc.tm_isdst = -1;

        // Convert to local time
        tm tm_local;
        time_t time_utc;
        #ifdef _WIN32
        time_utc = _mkgmtime(&tm_utc);
        localtime_s(&tm_local, &time_utc);
        #else
        time_utc = timegm(&tm_utc);
        localtime_r(&time_utc, &tm_local);
        #endif

        // Format local time as hh:mm
        std::ostringstream time_oss;
        time_oss << std::setfill('0') << std::setw(2) << tm_local.tm_hour
                 << ':' << std::setfill('0') << std::setw(2) << tm_local.tm_min;
        result.TimeHhMm = time_oss.str();

        const char* months[] = {"January", "February", "March", "April", "May", "June",
                               "July", "August", "September", "October", "November", "December"};
        std::ostringstream day_month_oss;
        day_month_oss << tm_local.tm_mday << ' ' << months[tm_local.tm_mon];
        result.DayMonth = day_month_oss.str();

        result.DayOfYear = tm_local.tm_yday + 1;

        return result;
    }
}
