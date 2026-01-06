#include "ClientUI.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <stdexcept>

namespace IMChat::Client
{
    ClientUI::ClientUI()
    {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize glfw!");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        m_Window = glfwCreateWindow(960, 720, "IMChat", nullptr, nullptr);
        if (!m_Window)
            throw std::runtime_error("Failed to create glfw window!");

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Failed to load opengl function pointers!");

        // Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    ClientUI::~ClientUI()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    bool ClientUI::WindowShouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }

    void ClientUI::BeginFrame()
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ClientUI::EndFrame()
    {
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_Window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_Window);
    }

    void ClientUI::DrawSimpleText(const std::string& text)
    {
        ImGui::SetNextWindowPos(
            ImGui::GetMainViewport()->GetCenter(),
            ImGuiCond_Always,
            ImVec2(0.5f, 0.5f)
        );

        ImGui::Text(text.c_str(), ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    }

    bool ClientUI::DrawErrorPopUp(const std::string& text)
    {
        bool ret = false;

        ImGui::OpenPopup("Error");

        ImGui::SetNextWindowPos(
            ImGui::GetMainViewport()->GetCenter(),
            ImGuiCond_Always,
            ImVec2(0.5f, 0.5f)
        );

        if (ImGui::BeginPopupModal("Error", nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::Text(text.c_str());
            ImGui::Spacing();

            if (ImGui::Button("OK"))
            {
                ret = true;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        return ret;
    }

    bool ClientUI::DrawLoginPopUp(std::string& username, std::string& password, const bool loginFailed)
    {
        static char username_arr[16] = {0};
        static char password_arr[24] = {0};
        bool ret = false;

        ImGui::OpenPopup("Login");

        // Center the popup
        ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);
        ImGui::SetNextWindowPos(
            ImGui::GetMainViewport()->GetCenter(),
            ImGuiCond_Always,
            ImVec2(0.5f, 0.5f)
        );

        if (ImGui::BeginPopupModal(
                "Login",
                nullptr,
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoCollapse))
        {
            ImGui::Text("Please log in");
            ImGui::Separator();

            // TODO: Input validation
            ImGui::InputText("Username", username_arr, IM_ARRAYSIZE(username_arr));
            ImGui::InputText("Password", password_arr, IM_ARRAYSIZE(password_arr),
                             ImGuiInputTextFlags_Password);

            if (loginFailed)
            {
                ImGui::TextColored(ImVec4(1,0,0,1), "Invalid username or password");
            }

            ImGui::Spacing();

            if (ImGui::Button("Login", ImVec2(120, 0)))
            {
                username = username_arr;
                password = password_arr;
                ret = true;
            }

            ImGui::EndPopup();
        }

        return ret;
    }

    bool ClientUI::DrawMainChatUI(const std::list<TextMessage>& messages, std::string& input)
    {
        return false;
    }
}
