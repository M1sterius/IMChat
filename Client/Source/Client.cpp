#include "Client.hpp"

#include <print>
#include <chrono>

#include "nlohmann_json/json.hpp"

namespace IMChat::Client
{
    Client::Client(const char* ip, const uint16_t port)
        : m_LoggedIn(false), m_AuthComplete(false)
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
        if (m_Worker.joinable())
            m_Worker.join();
    }

    void Client::Run()
    {
        if (!m_Connection->IsOpen())
        {
            std::println("Failed to connect to the server. Terminating client.");
            return;
        }

        // Auth loop
        while (!m_LoggedIn)
        {
            m_AuthComplete = false;
            std::println("Enter your credentials to log in.");

            const auto username = InputString("Enter username:", 5, 20, "\"!#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");
            const auto password = InputString("Enter password:", 5, 36, "\"!#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");

            auto json = nlohmann::json();
            json["Username"] = username;
            json["PasswordHash"] = SHA256(password);

            auto request = Message::MakeLoginRequest(json.dump());
            m_Connection->SendMessage(request);

            SleepUntil([this] { return !m_AuthComplete; }, 10000);
        }

        while (m_Connection->IsOpen())
        {
            const auto input = InputString("Enter text message:", 1, 500, "$%~");
            m_Connection->SendMessage(Message::MakeText(input));
        }

        std::println("Connection to the server has been lost!");
    }

    void Client::TryLogin()
    {
        std::println("You're not logged in! Please enter username and password.");

        const auto username = InputString("Enter username:", 5, 20, "\"!#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");
        const auto password = InputString("Enter password:", 5, 36, "\"!#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");

        auto json = nlohmann::json();
        json["Username"] = username;
        json["PasswordHash"] = SHA256(password);

        auto request = Message::MakeLoginRequest(json.dump());
        m_Connection->SendMessage(request);
    }

    void Client::SendMessage(const Message& message)
    {
        m_Connection->SendMessage(message);
    }

    void Client::OnReceiveMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message)
    {
        switch (message->Header.Type)
        {
            case MessageType::LoginResponse:
                ProcessLoginResponse(connection, message);
                break;
            default:
                std::println("Received invalid message");
        }
    }

    void Client::ProcessLoginResponse(std::shared_ptr<Connection> connection, const std::shared_ptr<Message>& message)
    {
        if (m_LoggedIn)
            return;

        const auto json = ParseJson(message->Body, message->Header.Size);

        const auto response = json["Response"].get<std::string>();
        const auto reason = json["Reason"].get<std::string>();

        if (response == "Approved")
        {
            std::println("Logged in successfully!");
            m_LoggedIn = true;
        }
        else
        {
            std::println("Login denied. {}", reason);
        }

        m_AuthComplete = true;
    }
}
