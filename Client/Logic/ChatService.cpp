#include "ChatService.hpp"

#include <iostream>

ChatService::ChatService() = default;
ChatService::~ChatService() = default;

void ChatService::onSendMessage(const QString& text)
{
    std::cout << text.toStdString() << '\n';
    m_MessageHistory.AddMessage({"You", text, QDateTime::currentDateTime()});
    emit updateMessageCount(m_MessageHistory.rowCount({}));
}
