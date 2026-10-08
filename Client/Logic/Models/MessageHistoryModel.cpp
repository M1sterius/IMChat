#include "MessageHistoryModel.hpp"

MessageHistoryModel::MessageHistoryModel(QObject* parent)
    : QAbstractListModel(parent) { }

MessageHistoryModel::~MessageHistoryModel() = default;

int MessageHistoryModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(m_Messages.size());
}

QVariant MessageHistoryModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_Messages.size())
        return {};

    const auto& message = m_Messages.at(index.row());
    const auto localTime = message.Timestamp.toLocalTime();

    switch (role)
    {
        case Role::AuthorRole:
            return message.Author;
        case Role::BodyRole:
            return message.Body;
        case Role::DayRole:
            return localTime.toString("yyyy-MM-dd");
        case Role::TimeLabelRole:
            return localTime.toString("HH:mm");
        default:
            return {};
    }
}

QHash<int, QByteArray> MessageHistoryModel::roleNames() const
{
    return {
        {AuthorRole, "author"},
        {BodyRole, "body"},
        {DayRole, "day"},
        {TimeLabelRole, "timeLabel"}
    };
}

void MessageHistoryModel::AddMessage(const Message& message)
{
    const auto row = static_cast<int>(m_Messages.size());

    beginInsertRows(QModelIndex(), row, row);
    m_Messages.append(message);
    endInsertRows();
}

void MessageHistoryModel::AddMessages(const QVector<Message>& messages)
{
    if (messages.isEmpty())
        return;

    const auto firstRow = static_cast<int>(m_Messages.size());
    const auto lastRow = firstRow + static_cast<int>(messages.size()) - 1;

    beginInsertRows(QModelIndex(), firstRow, lastRow);
    m_Messages.append(messages);
    endInsertRows();
}

void MessageHistoryModel::ClearAllMessages()
{
    if (m_Messages.isEmpty())
        return;

    beginResetModel();
    m_Messages.clear();
    endResetModel();
}
