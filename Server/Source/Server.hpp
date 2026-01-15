#pragma once

#include "Common.hpp"

#define ASIO_STANDALONE
#include "asio.hpp"
#include "pqxx/pqxx"
#include "nlohmann_json/json_fwd.hpp"

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
        static constexpr uint32_t MAX_CHAT_HISTORY_SEND_LENGTH = 100;

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

        void WaitForClientConnection();
        void OnReceiveMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);
        void OnClientDisconnect(std::shared_ptr<Connection> connection);

        void ProcessTextMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);
        void ProcessLoginRequest(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message);

        void PopulateLoginResponseInfo(nlohmann::json& json, const uint32_t connectionId);
        void SendChatHistoryUpdate(const uint32_t messageAuthorConnectionId, const std::string& message);
        void SendUsersListUpdate(const uint32_t userConnectionId, const std::string& username, const std::string& status);
    };
}