#pragma once

#include "Common.hpp"

#define ASIO_STANDALONE
#include "asio.hpp"

#include <thread>
#include <memory>
#include <vector>

namespace IMChat::Server
{
    class Server
    {
    private:
        struct ClientConnection
        {
            uint32_t ID;
            std::shared_ptr<asio::ip::tcp::socket> Socket;
        };
    public:
        explicit Server(const uint16_t port);
        ~Server();
    private:
        asio::io_context m_Context;
        std::thread m_Worker;
        asio::ip::tcp::acceptor m_Acceptor;
        std::shared_ptr<asio::ip::tcp::socket> m_Client;
        std::vector<char> m_Buffer;

        void WaitForClientConnection();
        void ReadData(const std::shared_ptr<asio::ip::tcp::socket>& socket, std::vector<char>& buffer);
    };
}