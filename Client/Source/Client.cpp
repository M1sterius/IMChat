#include "Client.hpp"

#include <print>

namespace IMChat::Client
{
    Client::Client(const char* ip, const uint16_t port)
        : m_Connection(asio::ip::tcp::socket(m_Context), 0)
    {
        auto idleWork = asio::make_work_guard(m_Context);
        m_Worker = std::thread([this] { m_Context.run(); });

        const auto server = asio::ip::tcp::endpoint(asio::ip::make_address(ip), port);
        m_Connection.GetSocket()->connect(server);

        // Use lambda or std::bind to pass a callback method
        m_Connection.SetReadMessageCallback([this](const Connection& client, std::shared_ptr<Message> msg)
        {
            this->OnReceiveMessage(client, msg);
        });
    }

    Client::~Client()
    {
        m_Context.stop();
        m_Worker.join();
    }

    bool Client::IsConnected() const
    {
        return m_Connection.IsOpen();
    }

    void Client::SendTextMessage(const std::string& text)
    {
        auto message = Message();
        message.Header.Type = MessageType::TestMessage;
        message.Header.Size = text.size();
        message.Body = std::vector(text.begin(), text.end());

        m_Connection.SendMessage(message);
    }

    void Client::OnReceiveMessage(const Connection& connection, std::shared_ptr<Message> message)
    {

    }
}
