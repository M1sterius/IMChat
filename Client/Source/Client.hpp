#pragma once

#include "Common.hpp"
#include "ClientUI.hpp"

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
        struct TextMessage
        {
            std::string Sender;
            std::string Timestamp;
            std::string Text;
        };
    public:
        Client(const char* ip, const uint16_t port);
        ~Client();

        void Run();
    private:
        asio::io_context m_Context;
        std::thread m_Worker;
        std::shared_ptr<Connection> m_Connection;
        std::string m_Username;
        bool m_LoggedIn;
        bool m_AuthComplete;

        std::unique_ptr<ClientUI> m_UI;

        void OnReceiveMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);

        void ProcessLoginResponse(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);
        void ProcessHistoryUpdate(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);
    };
}
