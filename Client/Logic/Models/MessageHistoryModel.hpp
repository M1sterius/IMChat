#pragma once

#include <QList>
#include <QObject>
#include <QAbstractListModel>

#include "../Message.hpp"

class MessageHistoryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit MessageHistoryModel(QObject *parent = nullptr);
    ~MessageHistoryModel() override;

    enum Role
    {
        AuthorRole = Qt::UserRole + 1,
        BodyRole,
        DayRole,
        TimeLabelRole
    };

    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void AddMessage(const Message& message);
    void AddMessages(const QVector<Message>& messages);
    void ClearAllMessages();
private:
    QVector<Message> m_Messages;
};
