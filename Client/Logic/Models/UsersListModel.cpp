#include "UsersListModel.hpp"

UsersListModel::UsersListModel(QObject* parent)
    : QAbstractListModel(parent) { }

UsersListModel::~UsersListModel() = default;

int UsersListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_Users.size());
}

QVariant UsersListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_Users.size())
        return {};

    switch (role)
    {
        case Role::NameRole:
            return m_Users.at(index.row());
        default:
            return {};
    }
}

QHash<int, QByteArray> UsersListModel::roleNames() const
{
    return {
        {NameRole, "name"}
    };
}

void UsersListModel::AddUser(const QString& name)
{
    const auto row = static_cast<int>(m_Users.size());

    beginInsertRows(QModelIndex(), row, row);
    m_Users.append(name);
    endInsertRows();
}

void UsersListModel::RemoveUser(const QString& name)
{
    const auto row = static_cast<int>(m_Users.indexOf(name));
    if (row < 0)
        return;

    beginRemoveRows(QModelIndex(), row, row);
    m_Users.removeAt(row);
    endRemoveRows();
}
