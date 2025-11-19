#include "Server.hpp"

#include <iostream>
#include <print>

namespace IMChat::Server
{
    Server::Server(const uint16_t port)
        : m_Acceptor(m_Context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), m_Buffer(KB(20))
    {
        WaitForClientConnection();
        m_Worker = std::thread([this] { m_Context.run(); });
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
                std::println("[SERVER] Client connected {}:{}.", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());

                m_Client = std::make_shared<asio::ip::tcp::socket>(std::move(socket));
                ReadData(m_Client, m_Buffer);
            }
            else
            {
                std::println("[SERVER] Failed to connect client at {}:{}.", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());
            }

            WaitForClientConnection();
        });
    }

    void Server::ReadData(const std::shared_ptr<asio::ip::tcp::socket>& socket, std::vector<char>& buffer)
    {
        // Schedule a single read operation!!!
        socket->async_read_some(asio::buffer(buffer), [this, &socket, &buffer](const asio::error_code ec, const size_t size)
        {
            if (!ec)
            {
                std::print("Received message: ");
                for (size_t i = 0; i < size; i++)
                    std::cout << buffer[i];
                std::cout << '\n';

                // Schedule another read operation in case the message wasn't received in full
                ReadData(socket, buffer);
            }
            else
                std::println("Error reading data: {}", ec.message());

        });
    }
}
