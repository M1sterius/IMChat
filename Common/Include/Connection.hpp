#pragma once

#include "Message.hpp"

#define ASIO_STANDALONE
#define _WIN32_WINNT 0x0A00
#include "asio.hpp"

#include <memory>
#include <functional>

// Do this because of stupid Windows API
#undef SendMessage

namespace IMChat
{
    /*
     * All instances of Connection MUST be managed by a std::shared_ptr and created via std::make_shared
     *
     * Make sure to call Start() when instantiating connection with an existing socket or
     * Connect() to have Connection automatically connect to an endpoint with given ipv4 and port.
     * Calling these methods is required to correctly schedule necessary async operations.
     */
    class Connection : public std::enable_shared_from_this<Connection>
    {
    public:
        explicit Connection(asio::ip::tcp::socket socket, const uint32_t id = 0);
        explicit Connection(asio::io_context& context, const uint32_t id = 0);
        ~Connection();

        void Start();
        void Connect(const char* ip, const uint16_t port);
        void Shutdown();

        bool IsConnected() const;

        uint32_t GetID() const { return m_ID; }
        void SetID(const uint32_t id) { m_ID = id; }

        asio::ip::tcp::socket& GetSocket() { return m_Socket; }
        const asio::ip::tcp::socket& GetSocket() const { return m_Socket; }

        void SendMessage(const Message& message);

        // Set a callback to be fired when a message is received.
        void SetReadMessageCallback(const std::function<void(std::shared_ptr<Connection>,
            std::shared_ptr<Message>)>& callback) { m_ReadCallback = callback; }
        // Set a callback to be fired when the connection gets closed or any other error occurs in any of the async ops
        void SetShutdownCallback(const std::function<void(std::shared_ptr<Connection>)>& callback) { m_DisconnectCallback = callback; }
    private:
        asio::ip::tcp::socket m_Socket;
        uint32_t m_ID;
        bool m_IsConnected{false};

        std::function<void(std::shared_ptr<Connection>, std::shared_ptr<Message>)> m_ReadCallback;
        std::function<void(std::shared_ptr<Connection>)> m_DisconnectCallback;

        void ReadMessageHeader(std::shared_ptr<Message> message);
        void ReadMessageBody(std::shared_ptr<Message> message);
    };
}
