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
        Client();
        ~Client();

        bool Connect(const char* ip, const uint16_t port);
        bool IsConnected() const;

        void SendMessage(const Message& message);
        void SendTextMessage(const std::string& text);
    private:
        asio::io_context m_Context;
        std::thread m_Worker;
        asio::ip::tcp::socket m_Socket;
    };
}