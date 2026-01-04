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

        m_Window = glfwCreateWindow(1280, 720, "IMChat", nullptr, nullptr);
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

        // Start ImGui frame
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

    bool ClientUI::DrawErrorPopUp(const std::string& text)
    {
        bool ret = false;

        ImGui::OpenPopup("My Popup");

        if (ImGui::BeginPopupModal("My Popup", nullptr,
            ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(text.c_str());
            ImGui::Spacing();

            float buttonWidth = 20.0f;
            float avail = ImGui::GetContentRegionAvail().x;
            ImGui::SetCursorPosX((avail - buttonWidth) * 0.5f);

            if (ImGui::Button("OK"))
            {
                ret = true;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        return ret;
    }
}
