import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

import "../Frontend"

ApplicationWindow {
    id: window
    width: 1024
    height: 576
    minimumWidth: 512
    minimumHeight: 288
    visible: true
    title: "IMChat"
    color: app_theme.background
    font.pixelSize: 14

    AppThemes {
        id: app_theme
    }

    Login {
        theme: app_theme
        service: chatService
    }

    RowLayout {
        visible: !chatService.showLogin
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        // title + chat history + input
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 80
            spacing: 8

            // settings + title
            RowLayout {
                Layout.preferredHeight: 10
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 8

                SettingsMenu {
                    id: settings_menu
                    theme: app_theme
                    Layout.preferredWidth: 7
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Rectangle {
                    color: app_theme.surface
                    radius: 10
                    border.color: app_theme.border
                    Layout.preferredWidth: 93
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 16
                        anchors.rightMargin: 16
                        spacing: 12

                        Text {
                            text: "IMChat"
                            color: app_theme.text
                            font.pixelSize: 16
                            font.weight: Font.DemiBold
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        // message count
                        Text {
                            text: message_history.messageCount === 0
                                ? qsTr("No messages")
                                : qsTr("Messages - %1").arg(message_history.messageCount)
                            color: app_theme.muted
                            font.pixelSize: 12
                        }
                    }
                }
            }

            MessageHistory {
                id: message_history
                Layout.preferredHeight: 80

                theme: app_theme
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            MessageInput {
                theme: app_theme
                Layout.preferredHeight: 10
                Layout.fillWidth: true

                onMessageSent: message_history.scrollToLatest()
            }
        }

        // list of active users
        ColumnLayout {
            Layout.preferredWidth: 20
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8

            // title
            Rectangle {
                color: app_theme.surface
                radius: 10
                border.color: app_theme.border
                Layout.preferredHeight: 10
                Layout.fillWidth: true
                Layout.fillHeight: true

                // connected users count
                Text {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    text: qsTr("Users - %1").arg(users_list.usersCount)
                    color: app_theme.text
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
            }

            UsersList {
                id: users_list
                theme: app_theme
            }
        }
    }
}
