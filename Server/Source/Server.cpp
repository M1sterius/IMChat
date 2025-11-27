#include "Server.hpp"

#include <iostream>
#include <print>

#include "nlohmann_json/json.hpp"

namespace IMChat::Server
{
    Server::Server(const uint16_t port)
        : m_Acceptor(m_Context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), m_IDs(100)
    {
        WaitForClientConnection();
        m_Worker = std::thread([this] { m_Context.run(); });

        std::println("[SERVER] Started.");
    }

    Server::~Server()
    {
        m_Context.stop();
        m_Worker.join();
    }

    void Server::Run()
    {
        std::println("[SERVER] Press any button to terminate the server.");
        std::cin.get();
    }

    void Server::WaitForClientConnection()
    {
        m_Acceptor.async_accept([this](const asio::error_code& ec, asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                std::println("[SERVER] Client connected at {}:{}.", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());
                m_Clients.emplace_back(std::make_shared<Connection>(std::move(socket), m_IDs++));

                m_Clients.back()->SetReadMessageCallback([this](const Connection& client, std::shared_ptr<Message> msg)
                {
                    this->OnReceiveMessage(client, msg);
                });

                // m_Clients.back().SetDisconnectCallback([this](const Connection& client)
                // {
                //     this->OnClientDisconnect(client);
                // });
            }
            else
            {
                std::println("[SERVER] Failed to connect client at {}:{}.", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());
            }

            WaitForClientConnection();
        });
    }

    void Server::OnReceiveMessage(const Connection& connection, const std::shared_ptr<Message>& message)
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
                std::println("Invalid message received from client {}!", connection.GetID());
        }
    }

    void Server::OnClientDisconnect(const Connection& connection)
    {
        const auto ID = connection.GetID();
        std::println("[SERVER] Client {} disconnected!", ID);
        m_Clients.remove_if([ID](const std::shared_ptr<Connection>& client) { return client->GetID() == ID; });
    }

    void Server::ProcessTextMessage(const Connection& connection, const std::shared_ptr<Message>& message)
    {
        std::print("[SERVER] Received text (Client {}, {} bytes): ", connection.GetID(), message->Header.Size);
        std::cout.write(message->Body.data(), message->Header.Size);
        std::cout << '\n';
    }

    void Server::ProcessLoginRequest(const Connection& connection, const std::shared_ptr<Message>& message)
    {
        std::println("[SERVER] Received login request from client {}.", connection.GetID());

        const auto json = nlohmann::json(message->Body);

        if (!json.contains("Username") || !json.contains("Password"))
        {
            // TODO: Send login denied response
        }

        const auto username = json["Username"].get<std::string>();
        const auto password = json["Password"].get<std::string>();
    }
}
