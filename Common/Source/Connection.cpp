#include "Connection.hpp"

#include <print>

namespace IMChat
{
    Connection::Connection(asio::ip::tcp::socket socket, const uint32_t ID)
        : m_Socket(std::make_shared<asio::ip::tcp::socket>(std::move(socket))), m_ID(ID)
    {
        ReadMessageHeader(std::make_shared<Message>());
    }

    Connection::~Connection() = default;

    bool Connection::IsOpen() const
    {
        return m_Socket->is_open();
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
        // Send header
        asio::async_write(*m_Socket, asio::buffer(&message.Header, sizeof(MessageHeader)),
            [](const asio::error_code ec, const std::size_t size)
        {
            if (ec)
                std::println("Failed to send message header!");
        });

        // Send body
        if (message.Header.Size > 0)
        {
            asio::async_write(*m_Socket, asio::buffer(message.Body.data(), message.Header.Size),
                [](const asio::error_code ec, const std::size_t size)
            {
                if (ec)
                    std::println("Failed to send message body!");
            });
        }
    }

    void Connection::Disconnect()
    {
        if (m_DisconnectCallback)
            m_DisconnectCallback(*this);
    }

    void Connection::ReadMessageHeader(std::shared_ptr<Message> message)
    {
        asio::async_read(*m_Socket, asio::buffer(&message->Header, sizeof(MessageHeader)),
            [this, message](const asio::error_code ec, const size_t size)
        {
            if (!ec)
                ReadMessageBody(message);
            else
                Disconnect();
        });
    }

    void Connection::ReadMessageBody(std::shared_ptr<Message> message)
    {
        message->Body.resize(message->Header.Size);

        asio::async_read(*m_Socket, asio::buffer(message->Body.data(), message->Header.Size),
            [this, message] (const asio::error_code ec, const size_t size)
        {
            if (!ec)
            {
                if (m_ReadCallback)
                    m_ReadCallback(*this, message);

                ReadMessageHeader(message);
            }
            else
                Disconnect();
        });
    }
}
