#pragma once

#include "Common.hpp"

#define ASIO_STANDALONE
#include "asio.hpp"

#include <thread>
#include <memory>
#include <list>

namespace IMChat::Server
{
    class Server
    {
    public:
        explicit Server(const uint16_t port);
        ~Server();
    private:
        asio::io_context m_Context;
        std::thread m_Worker;
        asio::ip::tcp::acceptor m_Acceptor;
        std::list<Connection> m_Clients;
        uint32_t m_IDs;

        void WaitForClientConnection();

        void OnReceiveMessage(const Connection& connection, std::shared_ptr<Message> message);
        void OnClientDisconnect(const Connection& connection);
    };
}