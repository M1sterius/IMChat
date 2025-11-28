#include "Client.hpp"

#include <iostream>
#include <print>

#include "nlohmann_json/json.hpp"

namespace IMChat::Client
{
    Client::Client(const char* ip, const uint16_t port)
        : m_IsLoggedIn(false)
    {
        auto idleWork = asio::make_work_guard(m_Context);
        m_Worker = std::thread([this] { m_Context.run(); });

        asio::ip::tcp::socket socket(m_Context);

        const auto server = asio::ip::tcp::endpoint(asio::ip::make_address(ip), port);
        socket.connect(server);

        m_Connection = Connection::Make(std::move(socket));

        // Use lambda or std::bind to pass a callback method
        m_Connection->SetReadMessageCallback([this](std::shared_ptr<Connection> client, std::shared_ptr<Message> msg)
        {
            this->OnReceiveMessage(client, msg);
        });
    }

    Client::~Client()
    {
        m_Context.stop();
        m_Worker.join();
    }

    void Client::Run()
    {
        // if (!m_IsLoggedIn)
        // {
        //     std::println("You're not logged in! Please enter username and password.");
        //
        //     std::print("Enter username:");
        //     const auto username = InputString();
        //
        //     std::print("Enter password:");
        //     const auto password = InputString();
        //
        //     auto json = nlohmann::json();
        //     json["Username"] = username;
        //     json["Password"] = password;
        //
        //     auto request = Message::MakeLoginRequest(json.dump());
        // }

        std::string line;
        while (m_Connection->IsOpen())
        {
            std::getline(std::cin, line);
            m_Connection->SendMessage(Message::MakeText(line));
        }

        std::println("Connection to the server has been lost!");
    }

    void Client::SendMessage(const Message& message)
    {
        m_Connection->SendMessage(message);
    }

    void Client::OnReceiveMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message)
    {

    }
}
