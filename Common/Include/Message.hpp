#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "nlohmann_json/json_fwd.hpp"

namespace IMChat
{
    enum class MessageType : uint32_t
    {
        Invalid,

        LoginRequest,
        LoginResponse,

        TextMessage,
        ChatHistoryUpdate,
        UsersListUpdate
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

        static Message Make(const std::string& string, const MessageType type);
        static Message Make(const nlohmann::json& json, const MessageType type);
    };
}
