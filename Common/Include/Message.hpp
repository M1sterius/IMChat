#pragma once

#include <cstdint>
#include <vector>

namespace IMChat
{
    enum class MessageType : uint32_t
    {
        Invalid,

        LoginRequest,
        LoginResponse,

        TestMessage,
    };

    struct MessageHeader
    {
        MessageType Type{MessageType::Invalid};
        uint32_t Size{0};
    };

    struct Message
    {
        MessageHeader Header;
        std::vector<char> Body;
    };
}