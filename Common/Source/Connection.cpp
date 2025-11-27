#include "Connection.hpp"

#include <print>

namespace IMChat
{
    Connection::Connection(asio::ip::tcp::socket socket, const uint32_t ID)
        : m_Socket(std::move(socket)), m_ID(ID)
    {
        ReadMessageHeader(std::make_shared<Message>());
    }

    Connection::~Connection() = default;

    bool Connection::IsOpen() const
    {
        return m_Socket.is_open();
    }

    void Connection::SetReadMessageCallback(const std::function<void(const Connection&, std::shared_ptr<Message>)>& callback)
    {
        m_ReadCallback = callback;
    }

    void Connection::SetDisconnectCallback(const std::function<void(const Connection&)>& callback)
    {
        m_DisconnectCallback = callback;
    }

    void Connection::SendMessage(const Message& message)
    {
        if (message.Header.Size < 1)
        {
            std::println("[CONNECTION] Sending empty messages is not allowed!");
            return;
        }

        // Send header
        asio::async_write(m_Socket, asio::buffer(&message.Header, sizeof(MessageHeader)),
            [](const asio::error_code ec, const std::size_t size)
        {
            if (ec)
                std::println("[CONNECTION] Failed to send message header!");
        });

        // Send body
        asio::async_write(m_Socket, asio::buffer(message.Body.data(), message.Header.Size),
            [](const asio::error_code ec, const std::size_t size)
        {
            if (ec)
                std::println("[CONNECTION] Failed to send message body!");
        });
    }

    void Connection::Disconnect()
    {
        // if (m_Socket->is_open())
        // {
        //     asio::error_code _;
        //     m_Socket->shutdown(asio::ip::tcp::socket::shutdown_both, _);
        //     m_Socket->close(_);
        // }

        std::println("[CONNECTION] Disconnect!");

        // if (m_DisconnectCallback)
        //     m_DisconnectCallback(*this);
    }

    void Connection::ReadMessageHeader(std::shared_ptr<Message> message)
    {
        auto self = shared_from_this();
        asio::async_read(m_Socket, asio::buffer(&message->Header, sizeof(MessageHeader)),
            [self, message](const asio::error_code ec, const size_t size)
        {
            if (!ec)
            {
                if (message->Header.Size > 0)
                    self->ReadMessageBody(message);
            }
            else
            {
                std::println("[CONNECTION] Error reading message header: {}", ec.message());
                self->Disconnect();
            }
        });
    }

    void Connection::ReadMessageBody(std::shared_ptr<Message> message)
    {
        message->Body.resize(message->Header.Size);

        auto self = shared_from_this();
        asio::async_read(m_Socket, asio::buffer(message->Body.data(), message->Header.Size),
            [self, message] (const asio::error_code ec, const size_t size)
        {
            if (!ec)
            {
                if (self->m_ReadCallback)
                    self->m_ReadCallback(*self, message);

                self->ReadMessageHeader(std::make_shared<Message>());
            }
            else
            {
                std::println("[CONNECTION] Error reading message body: {}", ec.message());
                self->Disconnect();
            }
        });
    }
}
