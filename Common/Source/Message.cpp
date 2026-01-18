#include "Message.hpp"

#include "nlohmann_json/json.hpp"

namespace IMChat
{
    Message Message::Make(const std::string& string, const MessageType type)
    {
        return Message{
            .Header = MessageHeader(type, string.size()),
            .Body = std::vector(string.begin(), string.end())
        };
    }

    Message Message::Make(const nlohmann::json& json, const MessageType type)
    {
        const auto string = json.dump();
        auto message = Message{
            .Header = MessageHeader(type, string.size()),
            .Body = std::vector(string.begin(), string.end())
        };

        return message;
    }
}
