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

                m_Clients.emplace_back(
                    m_IDs++,
                    std::make_shared<asio::ip::tcp::socket>(std::move(socket))
                );

                ReadMessageHeader(m_Clients.back(), std::make_shared<Message>());
            }
            else
            {
                std::println("[SERVER] Failed to connect client at {}:{}.", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());
            }

            WaitForClientConnection();
        });
    }

    void Server::DisconnectClient(const ClientConnection& connection)
    {
        const auto& rp = connection.Socket->remote_endpoint();

        std::println("[SERVER] Lost connection to client at {}:{}", rp.address().to_string(),
            rp.port());

        m_Clients.remove(connection);
    }

    void Server::ReadMessageHeader(ClientConnection& connection, std::shared_ptr<Message> message)
    {
        asio::async_read(connection.Socket, asio::buffer(&message->Header, sizeof(MessageHeader)),
            [this, &connection, &message](const asio::error_code ec, const size_t size)
        {
            if (!ec)
                ReadMessageBody(connection, message);
            else
                DisconnectClient(connection);
        });
    }

    void Server::ReadMessageBody(ClientConnection& connection, std::shared_ptr<Message> message)
    {
        message->Body.resize(message->Header.Size);

        asio::async_read(connection.Socket, asio::buffer(message->Body.data(), message->Header.Size),
            [this, &connection, &message] (const asio::error_code ec, const size_t size)
        {
            if (!ec)
            {
                ProcessMessage(connection, message);
                ReadMessageHeader(connection, message);
            }
            else
                DisconnectClient(connection);
        });
    }

    void Server::ProcessMessage(ClientConnection& connection, std::shared_ptr<Message> message)
    {

    }
}
