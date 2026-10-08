#pragma once

#include <QObject>

#include "Models/MessageHistoryModel.hpp"
#include "Models/UsersListModel.hpp"

class ChatService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool showLogin READ GetShowLogin WRITE SetShowLogin NOTIFY showLoginChanged)
public:
    ChatService();
    ~ChatService() override;

    MessageHistoryModel* GetMessageHistoryModel() { return &m_MessageHistory; }
    UsersListModel* GetUsersListModel() { return &m_Users; }

    bool GetShowLogin() const { return m_ShowLogin; }
    void SetShowLogin(const bool showLogin);

    Q_INVOKABLE void onSendMessage(const QString& text);
    Q_INVOKABLE void onTryLogin(const QString& username, const QString& password, const bool keepLoginCheck);
signals:
    void showLoginChanged();
    void updateMessageCount(int count);
private:
    MessageHistoryModel m_MessageHistory{};
    UsersListModel m_Users{};
    bool m_ShowLogin{true};
};
