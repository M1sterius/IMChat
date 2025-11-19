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

    void Client::SendData(const void* data, const size_t size)
    {
        m_Socket.async_write_some(asio::buffer(data, size), [](const asio::error_code ec, const size_t size) {});
    }

    void Client::SendData(const std::string& str)
    {
        SendData(str.data(), str.size());
    }
}
