#pragma once

#include "TextMessage.hpp"
#include "Common.hpp"

#include <string>
#include <list>

struct GLFWwindow;

namespace IMChat::Client
{
    class ClientUI
    {
    public:
        ClientUI();
        ~ClientUI();

        bool WindowShouldClose() const;

        void BeginFrame();
        void EndFrame();

        bool DrawPopUp(const std::string& label, const std::string& text, const bool okButton); // returns true when user clicks OK
        bool DrawLoginWindow(std::string& username, std::string& password, const bool loginFailed, const std::string& failureReason); // returns true when user clicks Login
        bool DrawMainChatUI(const std::list<TextMessage>& messages, const std::list<std::string>& connectedUsers, std::string& input); // returns true when user clicks SEND
    private:
        GLFWwindow* m_Window;

        bool DrawChatSubwindow(const std::list<TextMessage>& messages, std::string& input);
        void DrawUsersListSubwindow(const std::list<std::string>& connectedUsers);
    };
}
