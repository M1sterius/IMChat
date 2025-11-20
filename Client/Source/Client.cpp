#include "Client.hpp"

#include <print>

namespace IMChat::Client
{
    Client::Client()
        : m_Socket(m_Context)
    {
        auto idleWork = asio::make_work_guard(m_Context);
        m_Worker = std::thread([this] { m_Context.run(); });
    }

    Client::~Client()
    {
        m_Context.stop();
        m_Worker.join();
    }

    bool Client::Connect(const char* ip, const uint16_t port)
    {
        asio::error_code ec;

        const auto server = asio::ip::tcp::endpoint(asio::ip::make_address(ip), port);
        ec = m_Socket.connect(server, ec);

        if (!ec)
            std::println("Successfully connected to server at {}:{}", ip, port);
        else
        {
            std::println("Failed to connect to the server. Error: {}", ec.message());
            return false;
        }

        return true;
    }

    bool Client::IsConnected() const
    {
        return m_Socket.is_open();
    }

    void Client::SendMessage(const Message& message)
    {
        // Send header
        asio::async_write(m_Socket, asio::buffer(&message.Header, sizeof(MessageHeader)),
            [](const asio::error_code ec, const std::size_t size)
        {
            if (ec)
                std::println("Failed to send message header!");
        });

        // Send body
        if (message.Header.Size > 0)
        {
            asio::async_write(m_Socket, asio::buffer(message.Body.data(), message.Header.Size),
                [](const asio::error_code ec, const std::size_t size)
            {
                if (ec)
                    std::println("Failed to send message body!");
            });
        }
    }

    void Client::SendTextMessage(const std::string& text)
    {
        auto message = Message();
        message.Header.Type = MessageType::TestMessage;
        message.Header.Size = text.size();
        message.Body = std::vector(text.begin(), text.end());

        SendMessage(message);
    }
}
