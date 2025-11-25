#pragma once

#include "Common.hpp"

#define ASIO_STANDALONE
#include "asio.hpp"

#include <thread>
#include <string>

// Do this because of stupid Windows API
#undef SendMessage

namespace IMChat::Client
{
    class Client
    {
    public:
        Client(const char* ip, const uint16_t port);
        ~Client();

        bool IsConnected() const;

        void SendTextMessage(const std::string& text);
    private:
        asio::io_context m_Context;
        std::thread m_Worker;
        Connection m_Connection;

        void OnReceiveMessage(const Connection& connection, std::shared_ptr<Message> message);
    };
}