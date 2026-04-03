#include "Client.hpp"

#include "nlohmann_json/json.hpp"
#include "fmt/format.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

namespace IMChat::Client
{
    Client::Client(const char* ip, const uint16_t port)
        : m_LoginRequestSentTime(0), m_LoggedIn(false), m_LoginRequestSent(false), m_LoginFailed(false)
    {
        m_UI = std::make_unique<ClientUI>();

        #if defined(_WIN32) && !defined(_DEBUG)
        FreeConsole();
        #endif
        FreeConsole();

        auto idleWork = asio::make_work_guard(m_Context);
        m_Worker = std::thread([this] { m_Context.run(); });

        m_Connection = std::make_shared<Connection>(m_Context);

        try
        {
            m_Connection->Connect(ip, port);

            m_Connection->SetReadMessageCallback([this](std::shared_ptr<Connection> client, std::shared_ptr<Message> msg)
            {
                this->OnReceiveMessage(client, msg);
            });
        } catch (const std::exception& e) {
            fmt::println("[CONNECTION] Failed to connect to the server! Error:{}", e.what());
        }
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

            if (!m_LoggedIn && m_LoginRequestSent &&
                (time(nullptr) - m_LoginRequestSentTime) > MAX_SERVER_RESPONSE_WAIT_TIME)
            {
                m_LoginFailed = true;
                m_LoginRequestSent = false;
                m_LoginFailureReason = "No response from server!";
            }

            if (!m_Connection || !m_Connection->IsConnected()) // No server connection
            {
                if (m_UI->DrawPopUp("Error", "Failed to connect to the server!", true))
                    return;
            }
            else if (!m_LoggedIn && m_LoginRequestSent) // Waiting for server response to login request
            {
                m_UI->DrawPopUp("Waiting", "Awaiting server response...", false);
            }
            else if (!m_LoggedIn) // Login window
            {
                static std::string password;
                if (m_UI->DrawLoginWindow(m_Username, password, m_LoginFailed, m_LoginFailureReason))
                {
                    m_LoginRequestSent = true;
                    m_LoginFailed = false;
                    m_LoginRequestSentTime = time(nullptr);

                    auto json = nlohmann::json();
                    json["Username"] = m_Username;
                    json["PasswordHash"] = SHA256(password);

                    const auto request = Message::Make(json, MessageType::LoginRequest);
                    m_Connection->SendMessage(request);
                }
            }
            else // Main chat UI
            {
                static std::string input;
                if (m_UI->DrawMainChatUI(m_ChatHistory, m_ConnectedUsers, input))
                {
                    m_Connection->SendMessage(Message::Make(input, MessageType::TextMessage));

                    if (m_ChatHistory.size() >= MAX_CHAT_HISTORY_LENGTH)
                        m_ChatHistory.erase(m_ChatHistory.begin());

                    // Timestamp won't be precisely synced with db but it`s okay.
                    // Next time messages are loaded from server the db timestamp will be used
                    m_ChatHistory.emplace_back("You", TimestampTZ(), input);
                }
            }

            m_UI->EndFrame();
        }
    }

    void Client::OnReceiveMessage(std::shared_ptr<Connection> connection, std::shared_ptr<Message> message)
    {
        const auto json = ParseJson(message->Body, message->Body.size());

        switch (message->Header.Type)
        {
        case MessageType::LoginResponse:
            {
                ProcessLoginResponse(json);
                break;
            }
        case MessageType::ChatHistoryUpdate:
            {
                ProcessChatHistoryUpdate(json);
                break;
            }
        case MessageType::UsersListUpdate:
            {
                ProcessUsersListUpdate(json);
                break;
            }
        default:
            fmt::println("Received invalid message");
        }
    }

    void Client::ProcessLoginResponse(const nlohmann::json& json)
    {
        if (m_LoggedIn)
            return;

        if (!json.contains("Response") || !json.contains("Reason"))
            return; // Corrupted response

        const auto response = json["Response"].get<std::string>();
        const auto reason = json["Reason"].get<std::string>();

        if (response == "Approved")
        {
            m_ConnectedUsers.push_front(fmt::format("{} (You)", m_Username));
            m_LoggedIn = true;

            ProcessLoginResponseInfo(json);
        }
        else
        {
            m_LoginFailureReason = reason;
            m_LoginFailed = true;
        }

        m_LoginRequestSent = false;
    }

    void Client::ProcessLoginResponseInfo(const nlohmann::json& json)
    {
        if (!json.contains("Messages") || !json.contains("Users"))
            return; // Corrupted response

        // Chat history
        for (const auto& textMessage : json["Messages"].get<nlohmann::json::array_t>())
        {
            if (!textMessage.contains("Sender") || !textMessage.contains("Timestamp") || !textMessage.contains("Text"))
                continue; // Corrupted message

            const auto sender = textMessage["Sender"].get<std::string>();
            const auto timestamp = textMessage["Timestamp"].get<std::string>();
            const auto text = textMessage["Text"].get<std::string>();

            if (m_ChatHistory.size() >= MAX_CHAT_HISTORY_LENGTH)
                m_ChatHistory.erase(m_ChatHistory.begin());

            m_ChatHistory.emplace_back(sender == m_Username ? "You" : sender, timestamp, text);
        }

        // Connected users list
        for (const auto& username : json["Users"].get<nlohmann::json::array_t>())
        {
            // Prevents usernames from being duplicated in the list when multiple clients login under the same credentials
            if (username != m_Username)
                m_ConnectedUsers.push_back(username);
        }
    }

    void Client::ProcessChatHistoryUpdate(const nlohmann::json& json)
    {
        if (!json.contains("Sender") || !json.contains("Timestamp") || !json.contains("Text"))
            return; // Corrupted message

        const auto sender = json["Sender"].get<std::string>();
        const auto timestamp = json["Timestamp"].get<std::string>();
        const auto text = json["Text"].get<std::string>();

        if (m_ChatHistory.size() >= MAX_CHAT_HISTORY_LENGTH)
            m_ChatHistory.erase(m_ChatHistory.begin());

        m_ChatHistory.emplace_back(sender == m_Username ? "You" : sender, timestamp, text);
    }

    void Client::ProcessUsersListUpdate(const nlohmann::json& json)
    {
        if (!json.contains("Username") || !json.contains("Status"))
            return; // Corrupted message

        const auto username = json["Username"].get<std::string>();
        const auto status = json["Status"].get<std::string>();

        if (status == "Connected")
        {
            // Prevents usernames from being duplicated in the list when multiple clients login under the same credentials
            if (username != m_Username)
                m_ConnectedUsers.push_back(username);
        }
        else if (status == "Disconnected")
        {
            if (username != m_Username)
                std::erase_if(m_ConnectedUsers, [&username](const std::string& itUsername) { return username == itUsername; });
        }
        else
        {
            fmt::println("[CLIENT] Invalid connection status for user {}", username);
        }
    }
}
