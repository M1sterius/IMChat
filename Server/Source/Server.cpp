#include "Server.hpp"

#include <iostream>
#include <print>

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

    void Server::WaitForClientConnection()
    {
        m_Acceptor.async_accept([this](const asio::error_code& ec, asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                std::println("[SERVER] Client connected at {}:{}.", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());
                m_Clients.emplace_back(std::move(socket), m_IDs++);

                m_Clients.back().SetReadMessageCallback([this](const Connection& client, std::shared_ptr<Message> msg)
                {
                    this->OnReceiveMessage(client, msg);
                });

                m_Clients.back().SetDisconnectCallback([this](const Connection& client)
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

    void Server::OnReceiveMessage(const Connection& connection, std::shared_ptr<Message> message)
    {
        std::print("Received message (Client {}, {} bytes): ", connection.GetID(), message->Header.Size);
        std::cout.write(reinterpret_cast<const char*>(message->Body.data()), message->Header.Size);
        std::cout << '\n';
    }

    void Server::OnClientDisconnect(const Connection& connection)
    {
        const auto ID = connection.GetID();
        std::println("Client {} disconnected!", ID);
        m_Clients.remove_if([ID](const Connection& client) { return client.GetID() == ID; });
    }

    // void Server::DisconnectClient(const ClientConnection& connection)
    // {
    //     const auto& rp = connection.Socket->remote_endpoint();
    //
    //     std::println("[SERVER] Lost connection to client at {}:{}", rp.address().to_string(),
    //         rp.port());
    //
    //     const auto id = connection.ID;
    //     m_Clients.remove_if([id](const ClientConnection& client) { return client.ID == id; });
    // }
}
