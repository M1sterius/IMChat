#include "Connection.hpp"

#include "fmt/format.h"

namespace IMChat
{
    Connection::Connection(asio::ip::tcp::socket socket, const uint32_t id)
        : m_Socket(std::move(socket)), m_ID(id)
    {
        // Async ops must be started only after Connection is fully constructed!
    }

    Connection::Connection(asio::io_context& context, const uint32_t id)
        : m_Socket(context), m_ID(id)
    {
        // Async ops must be started only after Connection is fully constructed!
    }

    Connection::~Connection() = default;

    void Connection::Start()
    {
        if (m_Socket.is_open())
        {
            ReadMessageHeader(std::make_shared<Message>());
            m_IsConnected = true;
        }
    }

    void Connection::Connect(const char* ip, const uint16_t port)
    {
        m_Socket.connect(asio::ip::tcp::endpoint(asio::ip::make_address(ip), port));

        if (m_Socket.is_open())
        {
            ReadMessageHeader(std::make_shared<Message>());
            m_IsConnected = true;
        }
    }

    bool Connection::IsConnected() const
    {
        return m_IsConnected;
    }

    void Connection::Shutdown()
    {
        m_IsConnected = false;

        asio::error_code ec;
        ec = m_Socket.cancel(ec);
        ec = m_Socket.close(ec);

        if (m_DisconnectCallback)
            m_DisconnectCallback(shared_from_this());
    }

    // TODO: const& here is fucking unsafe. FIX IT!!!
    void Connection::SendMessage(const Message& message)
    {
        auto self = shared_from_this();

        if (message.Header.Size < 1)
        {
            fmt::println("[CONNECTION] Sending empty messages is not allowed!");
            return;
        }

        // Send header
        asio::async_write(m_Socket, asio::buffer(&message.Header, sizeof(MessageHeader)),
            [self](const asio::error_code ec, const std::size_t size)
        {
            if (ec)
            {
                fmt::println("[CONNECTION] Failed to send message header!");
                self->Shutdown();
            }
        });

        // Send body
        asio::async_write(m_Socket, asio::buffer(message.Body.data(), message.Header.Size),
            [self](const asio::error_code ec, const std::size_t size)
        {
            if (ec)
            {
                fmt::println("[CONNECTION] Failed to send message body!");
                self->Shutdown();
            }
        });
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
                fmt::println("[CONNECTION] Error reading message header: {}", ec.message());
                self->Shutdown();
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
                    self->m_ReadCallback(self, message);

                self->ReadMessageHeader(std::make_shared<Message>());
            }
            else
            {
                fmt::println("[CONNECTION] Error reading message body: {}", ec.message());
                self->Shutdown();
            }
        });
    }
}
