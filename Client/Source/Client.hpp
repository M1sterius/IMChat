#pragma once

#include "Common.hpp"
#include "ClientUI.hpp"
#include "TextMessage.hpp"

#define ASIO_STANDALONE
#include "asio.hpp"

#include <thread>

// Do this because of stupid Windows API
#undef SendMessage

namespace IMChat::Client
{
    class Client
    {
    private:
        static constexpr auto MAX_MESSAGE_HISTORY_LENGTH = 100u;
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
        std::list<TextMessage> m_MessageHistory;
        bool m_LoggedIn; // true if client is fully logged in and able to send messages
        bool m_AuthComplete; // true when login request is sent to the server and client should await the response
        bool m_LoginFailed; // true if user provided wrong username or password

        void OnReceiveMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);

        void ProcessLoginResponse(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);
        void ProcessHistoryUpdate(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);
    };
}
