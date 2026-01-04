#include "Client.hpp"

#include <print>
#include <chrono>
#include <iostream>

#include "nlohmann_json/json.hpp"

#ifdef _WIN32
#include "windows.h"
#endif

namespace IMChat::Client
{
    Client::Client(const char* ip, const uint16_t port)
        : m_LoggedIn(false), m_AuthComplete(false)
    {
        try
        {
            m_UI = std::make_unique<ClientUI>();

            #ifdef _WIN32
            FreeConsole();
            #endif
        } catch (const std::exception& e) {
            std::println("Failed to initialize client UI. Error: {}", e.what());
            return;
        }

        asio::error_code ec;
        auto idleWork = asio::make_work_guard(m_Context);
        m_Worker = std::thread([this] { m_Context.run(); });

        asio::ip::tcp::socket socket(m_Context);
        socket.connect(asio::ip::tcp::endpoint(asio::ip::make_address(ip), port), ec);

        if (ec)
            return;

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
        if (!m_UI)
            return;

        while (!m_UI->WindowShouldClose())
        {
            m_UI->BeginFrame();

            if (!m_Connection || !m_Connection->IsOpen())
            {
                if (m_UI->DrawErrorPopUp("Failed to connect to the server!"))
                    return;
            }

            m_UI->EndFrame();
        }

        // if (!m_Connection || !m_Connection->IsOpen())
        // {
        //     std::println("Failed to connect to the server. Terminating client.");
        //     return;
        // }
        //
        // // Auth loop
        // while (!m_LoggedIn)
        // {
        //     m_AuthComplete = false;
        //     std::println("Enter your credentials to log in.");
        //
        //     m_Username = InputString("Enter username:", 5, 20, "\"!#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");
        //     const auto password = InputString("Enter password:", 5, 36, "\"!#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");
        //
        //     auto json = nlohmann::json();
        //     json["Username"] = m_Username;
        //     json["PasswordHash"] = SHA256(password);
        //
        //     auto request = Message::MakeLoginRequest(json.dump());
        //     m_Connection->SendMessage(request);
        //
        //     SleepUntil([this] { return !m_AuthComplete; }, 10000);
        // }
        //
        // while (m_Connection->IsOpen())
        // {
        //     const auto input = InputString("", 1, MAX_TEXT_MESSAGE_LENGTH, "");
        //     m_Connection->SendMessage(Message::MakeText(input));
        // }
        //
        // std::println("Connection to the server has been lost!");
    }

    void Client::OnReceiveMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message)
    {
        switch (message->Header.Type)
        {
        case MessageType::LoginResponse:
            ProcessLoginResponse(connection, message);
            break;
        case MessageType::ChatHistoryUpdate:
            ProcessHistoryUpdate(connection, message);
            break;
        default:
            std::println("Received invalid message");
        }
    }

    void Client::ProcessLoginResponse(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message)
    {
        if (m_LoggedIn)
            return;

        const auto json = ParseJson(message->Body, message->Header.Size);

        const auto response = json["Response"].get<std::string>();
        const auto reason = json["Reason"].get<std::string>();

        if (response == "Approved")
        {
            std::println("Logged in successfully! Type you messages below!");
            m_LoggedIn = true;
        }
        else
        {
            std::println("Login denied. {}", reason);
        }

        m_AuthComplete = true;
    }

    void Client::ProcessHistoryUpdate(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message)
    {
        const auto json = ParseJson(message->Body, message->Header.Size);

        if (!json.contains("Messages"))
            return; // Corrupted message

        for (const auto& textMessage : json["Messages"].get<nlohmann::json::array_t>())
        {
            if (!textMessage.contains("Sender") || !textMessage.contains("Timestamp") || !textMessage.contains("Text"))
                continue; // Corrupted message

            const auto sender = textMessage["Sender"].get<std::string>();
            const auto timestamp = textMessage["Timestamp"].get<std::string>();
            const auto text = textMessage["Text"].get<std::string>();

            std::print("{}: {}\n", sender, text);
        }
    }
}
