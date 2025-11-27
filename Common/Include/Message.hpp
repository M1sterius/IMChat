#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace IMChat
{
    enum class MessageType : uint32_t
    {
        Invalid,

        LoginRequest,
        LoginResponse,

        TextMessage,
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

        static Message MakeText(const std::string& text)
        {
            auto message = Message();

            message.Header.Type = MessageType::TextMessage;
            message.Header.Size = static_cast<uint32_t>(text.size());
            message.Body = std::vector(text.begin(), text.end());

            return message;
        }

        static Message MakeLoginRequest(const std::string& requestJson)
        {
            auto message = Message();

            message.Header.Type = MessageType::LoginRequest;
            message.Header.Size = static_cast<uint32_t>(requestJson.size());
            message.Body = std::vector(requestJson.begin(), requestJson.end());

            return message;
        }

        static Message MakeLoginResponse(const std::string& responseJson)
        {
            auto message = Message();

            message.Header.Type = MessageType::LoginResponse;
            message.Header.Size = static_cast<uint32_t>(responseJson.size());
            message.Body = std::vector(responseJson.begin(), responseJson.end());

            return message;
        }
    };
}