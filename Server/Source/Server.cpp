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
                std::println("[SERVER] Failed to obtain db connection string environment variables.");
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

    void Server::OnReceiveMessage(std::shared_ptr<Connection> connection, const std::shared_ptr<Message>& message)
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
        const auto ID = connection->GetID();
        std::println("[SERVER] Client {} disconnected!", ID);
        m_Clients.erase(ID);
    }

    void Server::ProcessTextMessage(std::shared_ptr<Connection> connection, const std::shared_ptr<Message>& message)
    {
        if (const auto& client = m_Clients[connection->GetID()]; client.LoggedIn)
        {
            std::print("{}: ", client.Username);
            std::cout.write(message->Body.data(), message->Header.Size);
            std::cout << '\n';
        }
        else
        {
            std::println("[SERVER] Received a message from a not logged-in client. Connection ID: {}", connection->GetID());
        }
    }

    void Server::ProcessLoginRequest(std::shared_ptr<Connection> connection, const std::shared_ptr<Message>& message)
    {
        std::println("[SERVER] Received login request from client {}.", connection->GetID());

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
            const auto [qPasswordHash] = tx.query1<std::string>("SELECT password_hash FROM users WHERE username=$1;", username);

            if (qPasswordHash == passwordHash)
            {
                response["Response"] = "Approved";
                response["Reason"] = "";
                m_Clients[connection->GetID()].LoggedIn = true;
                m_Clients[connection->GetID()].Username = username;

                std::println("[SERVER] User '{}' logged in successfully", username);
            }
            else
            {
                response["Response"] = "Denied";
                response["Reason"] = "Wrong password";
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
