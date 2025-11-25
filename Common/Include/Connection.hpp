#pragma once

#include "Message.hpp"

#define ASIO_STANDALONE
#include "asio.hpp"

#include <memory>
#include <functional>

// Do this because of stupid Windows API
#undef SendMessage

namespace IMChat
{
    class Connection
    {
    public:
        explicit Connection(asio::ip::tcp::socket socket, const uint32_t ID = 0);
        ~Connection();

        bool IsOpen() const;
        uint32_t GetID() const { return m_ID; }
        const std::shared_ptr<asio::ip::tcp::socket>& GetSocket() const { return m_Socket; }

        void SendMessage(const Message& message);

        void SetReadMessageCallback(const std::function<void(const Connection&, std::shared_ptr<Message>)>& callback);
        void SetDisconnectCallback(const std::function<void(const Connection&)>& callback);
    private:
        std::shared_ptr<asio::ip::tcp::socket> m_Socket;
        uint32_t m_ID;

        std::function<void(const Connection&, std::shared_ptr<Message>)> m_ReadCallback;
        std::function<void(const Connection&)> m_DisconnectCallback;

        void Disconnect();
        void ReadMessageHeader(std::shared_ptr<Message> message);
        void ReadMessageBody(std::shared_ptr<Message> message);
    };
}