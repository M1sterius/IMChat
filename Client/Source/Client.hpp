#pragma once

#include "Common.hpp"

#define ASIO_STANDALONE
#include "asio.hpp"

#include <thread>
#include <string>

namespace IMChat::Client
{
    class Client
    {
    public:
        Client();
        ~Client();

        bool Connect(const char* ip, const uint16_t port);
        bool IsConnected() const;

        void SendData(const void* data, const size_t size);
        void SendData(const std::string& str);
    private:
        asio::io_context m_Context;
        std::thread m_Worker;
        asio::ip::tcp::socket m_Socket;
    };
}