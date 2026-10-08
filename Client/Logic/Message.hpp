#pragma once

#include <QString>
#include <QDateTime>

struct Message
{
    QString Author;
    QString Body;
    QDateTime Timestamp;
};