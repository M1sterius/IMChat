#pragma once

#include <string>

namespace IMChat::Client
{
    struct TextMessage
    {
        std::string Sender;
        std::string Timestamp;
        std::string Text;
    };
}
