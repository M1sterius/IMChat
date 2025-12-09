#pragma once

#include "Common.hpp"

#define ASIO_STANDALONE
#include "asio.hpp"

#include <thread>

// Do this because of stupid Windows API
#undef SendMessage

namespace IMChat::Client
{
    class Client
    {
    public:
        Client(const char* ip, const uint16_t port);
        ~Client();

        void Run();
    private:
        asio::io_context m_Context;
        std::thread m_Worker;
        std::shared_ptr<Connection> m_Connection;
        bool m_LoggedIn;
        bool m_AuthComplete;

        void TryLogin();

        void SendMessage(const Message& message);
        void OnReceiveMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);

        void ProcessLoginResponse(std::shared_ptr<Connection> connection, const std::shared_ptr<Message>& message);
    };
}
