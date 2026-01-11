#include "ClientUI.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <stdexcept>
#include <unordered_set>

namespace IMChat::Client
{
    static int UsernameInputFilter(ImGuiInputTextCallbackData* data)
    {
        static std::unordered_set<char> allowedCharacters;
        if (allowedCharacters.empty())
        {
            const auto len = strlen(USERNAME_ALLOWED_CHARACTERS_STRING);
            for (uint32_t i = 0; i < len; i++)
                allowedCharacters.insert(USERNAME_ALLOWED_CHARACTERS_STRING[i]);
        }

        if (allowedCharacters.contains(data->EventChar))
            return 0;

        return 1;
    }

    static int PasswordInputFilter(ImGuiInputTextCallbackData* data)
    {
        static std::unordered_set<char> allowedCharacters;
        if (allowedCharacters.empty())
        {
            const auto len = strlen(PASSWORD_ALLOWED_CHARACTERS_STRING);
            for (uint32_t i = 0; i < len; i++)
                allowedCharacters.insert(PASSWORD_ALLOWED_CHARACTERS_STRING[i]);
        }

        if (allowedCharacters.contains(data->EventChar))
            return 0;

        return 1;
    }

    // TODO: Text wrapping
    static int TextInputFilter(ImGuiInputTextCallbackData* data)
    {
        // if (++characterCount > MAX_TEXT_MESSAGE_LENGTH)
        //     return 1;

        return 0;
    }

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

        // TODO: Support other character ranges
        auto& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("Assets/Fonts/JetBrainsMono-Regular.ttf",
            18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());

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
            ImGui::Text("%s", text.c_str());
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
        // no *4 because username and password use only ASCII characters
        static char username_arr[MAX_USERNAME_LENGTH + 1] = {0};
        static char password_arr[MAX_PASSWORD_LENGTH + 1] = {0};
        bool ret = false;

        const auto popUpWidth = std::max(ImGui::GetMainViewport()->Size.x * 0.28f, 300.0f);

        ImGui::OpenPopup("Login");

        ImGui::SetNextWindowSize(ImVec2(popUpWidth, 0), ImGuiCond_Always);
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

            ImGui::InputText("Username", username_arr, IM_ARRAYSIZE(username_arr),
                ImGuiInputTextFlags_CallbackCharFilter, UsernameInputFilter);
            ImGui::InputText("Password", password_arr, IM_ARRAYSIZE(password_arr),
                ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CallbackCharFilter, PasswordInputFilter);

            if (loginFailed)
                ImGui::TextColored(ImVec4(1,0,0,1), "Invalid username or password");

            ImGui::Spacing();

            ImGui::BeginDisabled(strlen(username_arr) < MIN_USERNAME_LENGTH || strlen(password_arr) < MIN_PASSWORD_LENGTH);
            if (ImGui::Button("Login", ImVec2(120, 0)))
            {
                username = username_arr;
                password = password_arr;
                ret = true;
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }

        return ret;
    }

    bool ClientUI::DrawMainChatUI(const std::list<TextMessage>& messages, const std::list<std::string>& connectedUsers, std::string& input)
    {
        // create *4 + 1 buffer to make sure that all possible utf8 codepoints fit
        static char input_arr[MAX_TEXT_MESSAGE_LENGTH * 4 + 1] = {0};
        static bool scrollToBottom = false;
        bool ret = false;

        const auto viewport = ImGui::GetMainViewport();
        const auto viewportPos  = viewport->Pos;
        const auto viewportSize = viewport->Size;
        const auto chatWindowWidth = viewportSize.x * 0.8f;
        const auto sendButtonWidth = chatWindowWidth * 0.1f;
        const auto inputBoxHeight = ImGui::GetTextLineHeight() * 3 + ImGui::GetStyle().ItemSpacing.y * 2;

        ImGui::SetNextWindowPos(viewportPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(chatWindowWidth, viewportSize.y), ImGuiCond_Always);
        ImGui::Begin("IMChat", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

        // Message history
        ImGui::BeginChild(
            "History",
            ImVec2(0, -inputBoxHeight - ImGui::GetStyle().ItemSpacing.y * 2),
            true
        );

        for (const auto& msg : messages)
        {
            // TODO: Timestamp
            ImGui::TextWrapped("%s: %s",
                msg.Sender.c_str(),
                msg.Text.c_str());
            ImGui::Spacing();
        }

        // Auto-scroll
        if (scrollToBottom)
        {
            ImGui::SetScrollHereY(1.0f);
            scrollToBottom = false;
        }

        ImGui::EndChild();
        ImGui::Separator();

        // Multiline input
        // ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - sendButtonWidth - ImGui::GetStyle().ItemSpacing.x);

        // TODO: Text length limit
        bool send = ImGui::InputTextMultiline(
            "##ChatInput",
            input_arr,
            IM_ARRAYSIZE(input_arr),
            ImVec2(ImGui::GetContentRegionAvail().x - sendButtonWidth - ImGui::GetStyle().ItemSpacing.x, inputBoxHeight),
            ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_WordWrap |
            ImGuiInputTextFlags_CtrlEnterForNewLine | ImGuiInputTextFlags_CallbackCharFilter, TextInputFilter
        );

        // ImGui::PopItemWidth();
        ImGui::SameLine();

        // Send button
        ImGui::BeginDisabled(input_arr[0] == '\0');
        if (ImGui::Button("Send", ImVec2(sendButtonWidth, inputBoxHeight)))
            send = true;
        ImGui::EndDisabled();

        if (send && input_arr[0] != '\0')
        {
            input = input_arr;
            input_arr[0] = '\0';
            scrollToBottom = true;
            ret = true;
            ImGui::SetKeyboardFocusHere(-1);
        }

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(chatWindowWidth, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(viewportSize.x * 0.2, viewportSize.y), ImGuiCond_Always);
        ImGui::Begin(std::format("Users - {}", connectedUsers.size()).c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

        for (const auto& user : connectedUsers)
        {
            ImGui::Text("%s", user.c_str());
            ImGui::Spacing();
        }

        ImGui::End();

        return ret;
    }
}
