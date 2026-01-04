#pragma once

struct GLFWwindow;

#include <string>

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

        bool DrawErrorPopUp(const std::string& text);
    private:
        GLFWwindow* m_Window;
    };
}
