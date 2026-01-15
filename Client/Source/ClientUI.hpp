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

        void DrawSimpleText(const std::string& text);
        bool DrawErrorPopUp(const std::string& text); // returns true when user clicks OK
        bool DrawLoginPopUp(std::string& username, std::string& password, const bool loginFailed, const std::string& failureReason); // returns true when user clicks Login
        bool DrawMainChatUI(const std::list<TextMessage>& messages, const std::list<std::string>& connectedUsers, std::string& input); // returns true when user clicks SEND
    private:
        GLFWwindow* m_Window;
    };
}
