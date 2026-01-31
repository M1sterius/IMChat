#pragma once

#include "Common.hpp"
#include "ClientUI.hpp"
#include "TextMessage.hpp"

#define ASIO_STANDALONE
#include "asio.hpp"
#include "nlohmann_json/json_fwd.hpp"

#include <thread>

// Do this because of stupid Windows API
#ifdef _WIN32
#undef SendMessage
#endif

namespace IMChat::Client
{
    class Client
    {
    private:
        static constexpr auto MAX_CHAT_HISTORY_LENGTH = 100u;
        static constexpr auto MAX_SERVER_RESPONSE_WAIT_TIME = 5u;
    public:
        Client(const char* ip, const uint16_t port);
        ~Client();

        void Run();
    private:
        std::unique_ptr<ClientUI> m_UI;
        asio::io_context m_Context;
        std::thread m_Worker;
        std::shared_ptr<Connection> m_Connection;
        std::string m_Username;
        std::list<TextMessage> m_ChatHistory;
        std::list<std::string> m_ConnectedUsers;
        std::string m_LoginFailureReason;
        std::time_t m_LoginRequestSentTime;
        bool m_LoggedIn; // true if client is fully logged in and able to send messages
        bool m_LoginRequestSent; // true when login request is sent to the server and client should await the response
        bool m_LoginFailed; // true if login was rejected for any reason

        void OnReceiveMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);

        void ProcessLoginResponse(const nlohmann::json& json);
        void ProcessLoginResponseInfo(const nlohmann::json& json);

        void ProcessChatHistoryUpdate(const nlohmann::json& json);
        void ProcessUsersListUpdate(const nlohmann::json& json);
    };
}
