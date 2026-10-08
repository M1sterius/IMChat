#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Logic/ChatService.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    ChatService chatService;
    QQmlApplicationEngine engine;

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
    );

    // Adding test data
    chatService.GetUsersListModel()->AddUser("wilson");
    chatService.GetUsersListModel()->AddUser("house");
    chatService.GetUsersListModel()->AddUser("kirill");

    chatService.GetMessageHistoryModel()->AddMessages({
        {"You", "Good morning!", QDateTime::fromString("2026-09-28T09:00:00", Qt::ISODate)},
        {"wilson", "Morning. Is the chat working?", QDateTime::fromString("2026-09-28T09:02:00", Qt::ISODate)},
        {"house", "I can see both messages.", QDateTime::fromString("2026-09-28T09:03:00", Qt::ISODate)},
        {"You", "Great, the first test passed.", QDateTime::fromString("2026-09-28T09:05:00", Qt::ISODate)},
        {"wilson", "Here is a message from another day.", QDateTime::fromString("2026-09-29T14:10:00", Qt::ISODate)},
        {"house", "There should be a new date heading above it.", QDateTime::fromString("2026-09-29T14:12:00", Qt::ISODate)},
        {"You", "Messages from the same day should stay together.", QDateTime::fromString("2026-09-29T14:15:00", Qt::ISODate)},
        {"wilson", "This is a longer message to check how the message history wraps text when the window is narrow. It should stay inside the message panel and leave enough room for the scrollbar.", QDateTime::fromString("2026-09-29T14:18:00", Qt::ISODate)},
        {"house", "Almost midnight.", QDateTime::fromString("2026-09-30T23:55:00", Qt::ISODate)},
        {"You", "Last message of September.", QDateTime::fromString("2026-09-30T23:59:00", Qt::ISODate)},
        {"wilson", "First message of October!", QDateTime::fromString("2026-10-01T00:00:00", Qt::ISODate)},
        {"house", "The date heading should change here.", QDateTime::fromString("2026-10-01T00:01:00", Qt::ISODate)},
        {"You", "Back online this morning.", QDateTime::fromString("2026-10-01T10:30:00", Qt::ISODate)},
        {"wilson", "Still the same date, so no extra heading.", QDateTime::fromString("2026-10-01T10:32:00", Qt::ISODate)},
        {"house", "A couple of days later.", QDateTime::fromString("2026-10-03T16:00:00", Qt::ISODate)},
        {"You", "Testing a gap between dates.", QDateTime::fromString("2026-10-03T16:04:00", Qt::ISODate)},
        {"wilson", "Short message.", QDateTime::fromString("2026-10-03T16:08:00", Qt::ISODate)},
        {"house", "Another message to give the list more content.", QDateTime::fromString("2026-10-03T16:10:00", Qt::ISODate)},
        {"You", "Testing the history scrollbar.", QDateTime::fromString("2026-10-06T19:20:00", Qt::ISODate)},
        {"wilson", "Try scrolling up to the older messages.", QDateTime::fromString("2026-10-06T19:22:00", Qt::ISODate)},
        {"house", "Then resize the window and check the wrapping.", QDateTime::fromString("2026-10-06T19:25:00", Qt::ISODate)},
        {"You", "New day, new messages.", QDateTime::fromString("2026-10-07T08:00:00", Qt::ISODate)},
        {"wilson", "The history now contains several date groups.", QDateTime::fromString("2026-10-07T08:02:00", Qt::ISODate)},
        {"house", "End of the test history.", QDateTime::fromString("2026-10-07T08:05:00", Qt::ISODate)}
    });

    engine.rootContext()->setContextProperty("chatService", &chatService);
    engine.rootContext()->setContextProperty("messageHistoryModel", chatService.GetMessageHistoryModel());
    engine.rootContext()->setContextProperty("usersListModel", chatService.GetUsersListModel());

    engine.loadFromModule("ChatUI", "Main");
    return QGuiApplication::exec();
}
