#pragma once

#include <QVector>
#include <QString>
#include <QAbstractListModel>

class UsersListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit UsersListModel(QObject* parent = nullptr);
    ~UsersListModel() override;

    enum Role
    {
        NameRole = Qt::UserRole + 1
    };

    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void AddUser(const QString& name);
    void RemoveUser(const QString& name);
private:
    QVector<QString> m_Users;
};
