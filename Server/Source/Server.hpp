#pragma once

#include "Common.hpp"

#define ASIO_STANDALONE
#include "asio.hpp"

#include <thread>
#include <memory>
#include <vector>
#include <list>

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
        std::list<ClientConnection> m_Clients;
        uint32_t m_IDs;

        void WaitForClientConnection();
        void DisconnectClient(const ClientConnection& connection);

        void ReadMessageHeader(ClientConnection& connection, std::shared_ptr<Message> message);
        void ReadMessageBody(ClientConnection& connection, std::shared_ptr<Message> message);
        void ProcessMessage(ClientConnection& connection, std::shared_ptr<Message> message);
    };
}