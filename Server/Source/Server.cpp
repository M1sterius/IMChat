#include "Server.hpp"

#include <iostream>
#include <format>
#include <print>

#include "nlohmann_json/json.hpp"

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
                std::println("[SERVER] Failed to obtain environment variables for db connection string.");
                return;
            }

            m_dbConnection = std::make_unique<pqxx::connection>(std::format(
                "user={} dbname={} password={}", user, dbname, password));
        }
        catch (const std::exception& e)
        {
            std::println("[SERVER] Failed to connect to the db. Error: {}.", e.what());
            return;
        }

        WaitForClientConnection();
        m_Worker = std::thread([this] { m_Context.run(); });

        std::println("[SERVER] Successfully connected to db.");
        std::println("[SERVER] Server started.");
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
            std::println("[SERVER] An error occurred during server startup. Server terminated.");
            return;
        }

        std::println("[SERVER] Server running.");
        std::println("[SERVER] Press any button to terminate the server.");
        std::cin.get();
    }

    void Server::WaitForClientConnection()
    {
        m_Acceptor->async_accept([this](const asio::error_code& ec, asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                std::println("[SERVER] Client connected at {}:{}.", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());

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
                std::println("[SERVER] Failed to connect client at {}:{}.", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());
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
                std::println("Invalid message received from client {}!", connection->GetID());
        }
    }

    void Server::OnClientDisconnect(std::shared_ptr<Connection> connection)
    {
        SendUsersListUpdate(connection, m_Clients[connection->GetID()].Username, "Disconnected");
        const auto ID = connection->GetID();
        std::println("[SERVER] Client {} ({}) disconnected!", ID, m_Clients[connection->GetID()].Username);
        m_Clients.erase(ID);
    }

    void Server::SendChatHistoryUpdate(std::shared_ptr<Connection> sender, std::shared_ptr<Message> message)
    {
        auto json = nlohmann::json();
        json["Messages"] = nlohmann::json::array();

        json["Messages"].push_back({
            {"Sender", m_Clients[sender->GetID()].Username},
            {"Timestamp", "TODO"},
            {"Text", std::string(message->Body.begin(), message->Body.end())}
        });

        const auto updateMessage = Message::MakeHistoryUpdate(json.dump());

        for (const auto& [id, client] : m_Clients)
        {
            if (id == sender->GetID())
                continue;

            client.Connection->SendMessage(updateMessage);
        }
    }

    void Server::SendFullChatHistory(std::shared_ptr<Connection> receiver, const uint32_t maxMessages)
    {
        auto message = nlohmann::json{
            {"Sender", ""},
            {"Timestamp", ""},
            {"Text", ""},
        };

        auto json = nlohmann::json();
        json["Messages"] = nlohmann::json::array();

        try
        {
            pqxx::work tx{*m_dbConnection};
            for (auto [username, text, timestamp] : tx.stream<std::string, std::string, std::string>(
                "SELECT users.username, messages.message, messages.timestamp FROM messages JOIN users ON messages.user_id = users.id ORDER BY timestamp ASC LIMIT " + std::to_string(maxMessages)))
            {
                message["Sender"] = username;
                message["Timestamp"] = timestamp;
                message["Text"] = text;

                json["Messages"].push_back(message);
            }
            tx.commit();

            receiver->SendMessage(Message::MakeHistoryUpdate(json.dump()));
        }
        catch (const std::exception& e)
        {
            std::println("[SERVER] Failed to query chat history for user {}. Error: {}.", receiver->GetID(), e.what());
        }
    }

    void Server::SendUsersListUpdate(std::shared_ptr<Connection> receiver, const std::string& username, const std::string& status)
    {
        auto json = nlohmann::json();
        json["Username"] = username;
        json["Status"] = status;

        for (const auto& [id, client] : m_Clients)
        {
            if (id == receiver->GetID())
                continue;

            client.Connection->SendMessage(Message::MakeUsersListUpdate(json.dump()));
        }
    }

    void Server::ProcessTextMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message)
    {
        if (const auto& client = m_Clients[connection->GetID()]; client.LoggedIn)
        {
            try
            {
                pqxx::work tx{*m_dbConnection};
                tx.exec_params("INSERT INTO messages (user_id, message, timestamp) VALUES ($1, $2, NOW())",
                    client.DatabaseID, std::string_view(message->Body.begin(), message->Body.end()));
                tx.commit();

                std::print("{}: ", client.Username);
                std::cout.write(message->Body.data(), message->Header.Size);
                std::cout << '\n';

                SendChatHistoryUpdate(connection, message);
            }
            catch (const std::exception& e)
            {
                std::println("[SERVER] Failed to add incoming message to db. Error: {}.", e.what());
            }
        }
        else
        {
            std::println("[SERVER] Received a message from an unauthenticated client. Connection ID: {}", connection->GetID());
        }
    }

    void Server::ProcessLoginRequest(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message)
    {
        std::println("[SERVER] Received login request from client {}.", connection->GetID());

        const auto connectionId = connection->GetID();
        const auto request = ParseJson(message->Body, message->Header.Size);
        auto response = nlohmann::json();

        if (!request.contains("Username") || !request.contains("PasswordHash"))
        {
            std::println("[SERVER] Received corrupted login request from client {}.", connection->GetID());

            response["Response"] = "Denied";
            response["Reason"] = "Corrupted request";
            connection->SendMessage(Message::MakeLoginResponse(response));

            return;
        }

        const auto username = request["Username"].get<std::string>();
        const auto passwordHash = request["PasswordHash"].get<std::string>();

        try
        {
            pqxx::work tx{*m_dbConnection};
            const auto [qID, qPasswordHash] = tx.query1<uint64_t, std::string>("SELECT id, password_hash FROM users WHERE username=$1", username);
            tx.commit();

            if (qPasswordHash == passwordHash)
            {
                response["Response"] = "Approved";
                response["Reason"] = "";
                m_Clients[connectionId].LoggedIn = true;
                m_Clients[connectionId].Username = username;
                m_Clients[connectionId].DatabaseID = qID;

                std::println("[SERVER] User '{}' logged in successfully", username);
                SendFullChatHistory(connection, 100);
                SendUsersListUpdate(connection, username, "Connected");
            }
            else
            {
                response["Response"] = "Denied";
                response["Reason"] = "Wrong password";
                std::println("[SERVER] Client {} provided wrong password for user {}.", connectionId, username);
            }
        }
        catch (const std::exception& e)
        {
            std::println("[SERVER] Failed to query data for user '{}'. Error: {}.", username, e.what());

            response["Response"] = "Denied";
            response["Reason"] = "User not found";
        }

        connection->SendMessage(Message::MakeLoginResponse(response.dump()));
    }
}
