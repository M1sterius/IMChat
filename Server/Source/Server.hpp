#pragma once

#include "Common.hpp"

#define ASIO_STANDALONE
#include "asio.hpp"
#include "pqxx/pqxx"

#include <thread>
#include <memory>
#include <unordered_map>

namespace IMChat::Server
{
    class Server
    {
    private:
        struct ClientConnection
        {
            std::shared_ptr<Connection> Connection;
            bool LoggedIn;
            std::string Username;
            uint64_t DatabaseID;
        };
    public:
        explicit Server(const uint16_t port);
        ~Server();

        void Run();
    private:
        asio::io_context m_Context;
        std::thread m_Worker;
        std::unique_ptr<asio::ip::tcp::acceptor> m_Acceptor;
        std::unique_ptr<pqxx::connection> m_dbConnection;
        std::unordered_map<uint32_t, ClientConnection> m_Clients;
        uint32_t m_IDs;
        bool m_StartupOK;

        void SendUpdateChatHistory(std::shared_ptr<Connection> sender, std::shared_ptr<Message> message);
        void SendChatHistory(std::shared_ptr<Connection> receiver, const uint32_t maxMessages);

        void WaitForClientConnection();
        void OnReceiveMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);
        void OnClientDisconnect(std::shared_ptr<Connection> connection);

        void ProcessTextMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);
        void ProcessLoginRequest(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);
    };
}