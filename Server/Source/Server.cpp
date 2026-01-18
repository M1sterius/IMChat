#include "Server.hpp"

#include "nlohmann_json/json.hpp"
#include "fmt/format.h"

#include <iostream>
#include <format>
#include <print>
#include <unordered_set>

namespace IMChat::Server
{
    Server::Server(const uint16_t port)
        : m_IDs(1), m_StartupOK(false)
    {
        m_Acceptor = std::make_unique<asio::ip::tcp::acceptor>(m_Context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));

        try
        {
            const auto user = GetEnv("IMCHAT_POSTGRESQL_USER");
            const auto dbname = GetEnv("IMCHAT_DB_NAME");
            const auto password = GetEnv("IMCHAT_DB_PASSWORD");

            if (user.empty() || dbname.empty() || password.empty())
            {
                fmt::println("[SERVER] Failed to obtain environment variables for db connection string.");
                return;
            }

            m_dbConnection = std::make_unique<pqxx::connection>(fmt::format(
                "user={} dbname={} password={}", user, dbname, password));
        }
        catch (const std::exception& e)
        {
            fmt::println("[SERVER] Failed to connect to the db. Error: {}.", e.what());
            return;
        }

        WaitForClientConnection();
        m_Worker = std::thread([this] { m_Context.run(); });

        fmt::println("[SERVER] Successfully connected to db.");
        fmt::println("[SERVER] Server started.");
        m_StartupOK = true;
    }

    Server::~Server()
    {
        m_Context.stop();
        if (m_Worker.joinable())
            m_Worker.join();
    }

    void Server::Run()
    {
        if (!m_StartupOK)
        {
            fmt::println("[SERVER] An error occurred during server startup. Server terminated.");
            return;
        }

        fmt::println("[SERVER] Server running.");
        fmt::println("[SERVER] Press any button to terminate the server.");
        std::cin.get();
    }

    void Server::WaitForClientConnection()
    {
        m_Acceptor->async_accept([this](const asio::error_code& ec, asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                fmt::println("[SERVER] Client connected at {}:{}.", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());

                const auto newId = m_IDs++;
                m_Clients[newId] = ClientConnection{
                    .Connection = Connection::Make(std::move(socket), newId),
                    .LoggedIn = false
                };

                m_Clients[newId].Connection->SetReadMessageCallback([this](std::shared_ptr<Connection> client, std::shared_ptr<Message> msg)
                {
                    this->OnReceiveMessage(client, msg);
                });
                m_Clients[newId].Connection->SetDisconnectCallback([this](std::shared_ptr<Connection> client)
                {
                    this->OnClientDisconnect(client);
                });
            }
            else
            {
                fmt::println("[SERVER] Failed to connect client at {}:{}.", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());
            }

            WaitForClientConnection();
        });
    }

    void Server::OnReceiveMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message)
    {
        switch (message->Header.Type)
        {
            case MessageType::TextMessage:
                ProcessTextMessage(connection, message);
                break;
            case MessageType::LoginRequest:
                ProcessLoginRequest(connection, message);
                break;
            default:
                fmt::println("[SERVER] Invalid message received from client {}!", connection->GetID());
        }
    }

    void Server::OnClientDisconnect(std::shared_ptr<Connection> connection)
    {
        const auto connectionId = connection->GetID();
        const auto& username = m_Clients[connectionId].Username;

        SendUsersListUpdate(connectionId, username, "Disconnected");
        fmt::println("[SERVER] Client {} ({}) disconnected!", connectionId, username);
        m_Clients.erase(connectionId);
    }

    void Server::SendChatHistoryUpdate(const uint32_t messageAuthorConnectionId, const std::string& message)
    {
        // Send info about a new text message to all users except the message author

        const auto json = nlohmann::json{
            {"Sender", m_Clients[messageAuthorConnectionId].Username},
            {"Timestamp", "TODO"},
            {"Text", message}
        };

        const auto update = Message::Make(json, MessageType::ChatHistoryUpdate);

        for (const auto& [id, client] : m_Clients)
        {
            if (id == messageAuthorConnectionId)
                continue;

            client.Connection->SendMessage(update);
        }
    }

    void Server::SendUsersListUpdate(const uint32_t userConnectionId, const std::string& username, const std::string& status)
    {
        // Send info about connected/disconnected status of a user to all other users

        auto json = nlohmann::json();
        json["Username"] = username;
        json["Status"] = status;

        for (const auto& [id, client] : m_Clients)
        {
            if (id == userConnectionId)
                continue;

            client.Connection->SendMessage(Message::Make(json, MessageType::UsersListUpdate));
        }
    }

    void Server::PopulateLoginResponseInfo(nlohmann::json& json, const uint32_t connectionId)
    {
        json["Messages"] = nlohmann::json::array();
        json["Users"] = nlohmann::json::array();

        // Chat history
        try
        {
            const auto query = std::format("SELECT username, message, timestamp FROM (SELECT users.username, "
                "messages.message, messages.timestamp FROM messages JOIN users ON messages.user_id=users.id "
                "ORDER BY messages.timestamp DESC LIMIT {}) ORDER BY timestamp ASC", MAX_CHAT_HISTORY_SEND_LENGTH);

            pqxx::work tx{*m_dbConnection};
            for (auto [username, text, timestamp] : tx.stream<std::string, std::string, std::string>(query))
            {
                json["Messages"].push_back({
                    {"Sender", username},
                    {"Timestamp", timestamp},
                    {"Text", text},
                });
            }
            tx.commit();
        }
        catch (const std::exception& e)
        {
            fmt::println("[SERVER] Failed to query chat history. Error: {}.", e.what());
        }

        // Connected users list
        std::unordered_set<std::string> usernamesSet;
        for (const auto& [id, client] : m_Clients)
        {
            if (id == connectionId)
                continue;

            // This prevents the same username from being added multiple times when more than 1 client is logged under
            // the same credentials
            if (!usernamesSet.contains(client.Username))
            {
                json["Users"].push_back(client.Username);
                usernamesSet.insert(client.Username);
            }
        }
    }

    void Server::ProcessTextMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message)
    {
        if (const auto& client = m_Clients[connection->GetID()]; client.LoggedIn)
        {
            const auto text = std::string(message->Body.begin(), message->Body.end());

            if (const auto textLength = text.length(); textLength > MAX_TEXT_MESSAGE_LENGTH)
            {
                fmt::println("[SERVER] Text message received from user '{}' is too long ({} > {})", client.Username, textLength, MAX_TEXT_MESSAGE_LENGTH);
                return;
            }

            try
            {
                pqxx::work tx{*m_dbConnection};
                tx.exec_params("INSERT INTO messages (user_id, message, timestamp) VALUES ($1, $2, NOW())",
                    client.DatabaseID, text);
                tx.commit();

                SendChatHistoryUpdate(connection->GetID(), text);
            }
            catch (const std::exception& e)
            {
                fmt::println("[SERVER] Failed to add incoming text message to db. Error: {}.", e.what());
            }
        }
        else
        {
            fmt::println("[SERVER] Received a message from an unauthenticated client. Connection ID: {}", connection->GetID());
        }
    }

    void Server::ProcessLoginRequest(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message)
    {
        fmt::println("[SERVER] Received login request from client {}.", connection->GetID());

        const auto connectionId = connection->GetID();
        const auto request = ParseJson(message->Body, message->Header.Size);
        auto response = nlohmann::json();

        if (!request.contains("Username") || !request.contains("PasswordHash"))
        {
            fmt::println("[SERVER] Received corrupted login request from client {}.", connection->GetID());

            response["Response"] = "Denied";
            response["Reason"] = "Corrupted request";
            connection->SendMessage(Message::Make(response, MessageType::LoginResponse));

            return;
        }

        const auto username = request["Username"].get<std::string>();
        const auto passwordHash = request["PasswordHash"].get<std::string>();

        try
        {
            pqxx::work tx{*m_dbConnection};
            const auto [qID, qPasswordHash] = tx.query1<uint64_t, std::string>("SELECT id, password_hash FROM users WHERE username=$1", username);
            tx.commit();

            if (passwordHash == qPasswordHash)
            {
                response["Response"] = "Approved";
                response["Reason"] = "";
                m_Clients[connectionId].LoggedIn = true;
                m_Clients[connectionId].Username = username;
                m_Clients[connectionId].DatabaseID = qID;

                fmt::println("[SERVER] User '{}' logged in successfully", username);
                PopulateLoginResponseInfo(response, connectionId);
                SendUsersListUpdate(connection->GetID(), username, "Connected");
            }
            else
            {
                response["Response"] = "Denied";
                response["Reason"] = "Wrong password";
                fmt::println("[SERVER] Client {} provided wrong password for user {}.", connectionId, username);
            }
        }
        catch (const std::exception& e)
        {
            fmt::println("[SERVER] Failed to query data for user '{}'. Error: {}.", username, e.what());

            response["Response"] = "Denied";
            response["Reason"] = "User not found";
        }

        connection->SendMessage(Message::Make(response, MessageType::LoginResponse));
    }
}
