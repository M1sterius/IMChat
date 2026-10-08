#pragma once

#include <QObject>

#include "Models/MessageHistoryModel.hpp"
#include "Models/UsersListModel.hpp"

class ChatService : public QObject
{
    Q_OBJECT
public:
    ChatService();
    ~ChatService() override;

    MessageHistoryModel* GetMessageHistoryModel() { return &m_MessageHistory; }
    UsersListModel* GetUsersListModel() { return &m_Users; }

    Q_INVOKABLE void onSendMessage(const QString& text);
signals:
    void updateMessageCount(int count);
private:
    MessageHistoryModel m_MessageHistory;
    UsersListModel m_Users;
};
