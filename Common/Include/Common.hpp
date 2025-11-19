#pragma once

#include <cstdint>
#include <vector>

typedef uint8_t byte_t;

#define IMCHAT_SERVER_IP "127.0.0.1"
#define IMCHAT_SERVER_PORT 9997

#define KB(n) (n * 1024)

namespace IMChat
{
    enum class MessageType
    {
        Login,
        Text
    };

    struct MessageHeader
    {
        MessageType Type;
        uint32_t Size;
    };

    struct Message
    {
        MessageHeader Header;
        std::vector<byte_t> Body;
    };
}
