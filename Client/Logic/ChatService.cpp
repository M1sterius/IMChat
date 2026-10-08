#include "ChatService.hpp"

#include <iostream>

ChatService::ChatService() = default;
ChatService::~ChatService() = default;

void ChatService::SetShowLogin(const bool showLogin)
{
    if (m_ShowLogin == showLogin)
        return;

    m_ShowLogin = showLogin;
    emit showLoginChanged();
}

void ChatService::onSendMessage(const QString& text)
{
    std::cout << text.toStdString() << '\n';
    m_MessageHistory.AddMessage({"You", text, QDateTime::currentDateTime()});
    emit updateMessageCount(m_MessageHistory.rowCount({}));
}

void ChatService::onTryLogin(const QString& username, const QString& password, const bool keepLoginCheck)
{
    std::cout << username.toStdString() << ':' << password.toStdString() << '\n';
}
