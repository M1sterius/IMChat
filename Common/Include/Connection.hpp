#pragma once

#include "Message.hpp"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include "asio.hpp"

#include <memory>
#include <functional>

// Do this because of stupid Windows API
#undef SendMessage

namespace IMChat
{
    class Connection : public std::enable_shared_from_this<Connection>
    {
    public:
        static std::shared_ptr<Connection> Make(asio::ip::tcp::socket socket, const uint32_t ID = 0);

        explicit Connection(asio::ip::tcp::socket socket, const uint32_t ID = 0);
        ~Connection();

        void Start();

        bool IsOpen() const;
        uint32_t GetID() const { return m_ID; }
        asio::ip::tcp::socket& GetSocket() { return m_Socket; }

        void SendMessage(const Message& message);

        void SetReadMessageCallback(const std::function<void(std::shared_ptr<Connection>, std::shared_ptr<Message>)>& callback);
        void SetDisconnectCallback(const std::function<void(std::shared_ptr<Connection>)>& callback);
    private:
        asio::ip::tcp::socket m_Socket;
        uint32_t m_ID;

        std::function<void(std::shared_ptr<Connection>, std::shared_ptr<Message>)> m_ReadCallback;
        std::function<void(std::shared_ptr<Connection>)> m_DisconnectCallback;

        void Disconnect();
        void ReadMessageHeader(std::shared_ptr<Message> message);
        void ReadMessageBody(std::shared_ptr<Message> message);
    };
}
