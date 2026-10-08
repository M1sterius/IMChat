import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

import "../Frontend"

ApplicationWindow {
    id: window
    width: 1024
    height: 576
    minimumWidth: 200
    minimumHeight: 250
    visible: true
    title: "IMChat"
    color: app_theme.background
    font.pixelSize: 14

    QtObject {
        id: app_theme

        readonly property bool lightMode: Application.styleHints.colorScheme === Qt.Light
        readonly property color background: lightMode ? "#f3dfcd" : "#30344d"
        readonly property color surface: lightMode ? "#ffe9d6" : "#3a3e5b"
        readonly property color subtle: lightMode ? "#f4decb" : "#444964"
        readonly property color border: lightMode ? "#d4bdad" : "#565a74"
        readonly property color text: lightMode ? "#3a3e5b" : "#ffe9d6"
        readonly property color muted: lightMode ? "#706577" : "#c5b9b3"
        readonly property color accent: lightMode ? "#3a3e5b" : "#ffe9d6"
        readonly property color button: lightMode ? "#3a3e5b" : "#ffe9d6"
        readonly property color buttonText: lightMode ? "#ffe9d6" : "#3a3e5b"
        readonly property color buttonHover: lightMode ? "#484d6c" : "#fff2e6"
        readonly property color buttonPressed: lightMode ? "#30344d" : "#ead1bc"
        // readonly property color online: lightMode ? "#657b62" : "#a8b69a"
        readonly property color online: lightMode ? "green" : "green"
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        // title + chat history + input
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 80
            spacing: 8

            // title
            Rectangle {
                color: app_theme.surface
                radius: 10
                border.color: app_theme.border
                Layout.preferredHeight: 10
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

            // Rectangle {
            //     color: app_theme.surface
            //     radius: 10
            //     border.color: app_theme.border
            //     Layout.preferredHeight: 90
            //     Layout.fillWidth: true
            //     Layout.fillHeight: true
            //
            //     ListView {
            //         id: users_list
            //
            //         anchors.fill: parent
            //         anchors.margins: 8
            //         spacing: 4
            //
            //         clip: true
            //         model: usersListModel
            //
            //         delegate: Rectangle {
            //             id: user_row
            //
            //             required property string name
            //
            //             width: Math.max(0, users_list.width - 12)
            //             height: 40
            //             radius: 6
            //             color: user_hover.hovered ? app_theme.subtle : "transparent"
            //
            //             HoverHandler {
            //                 id: user_hover
            //             }
            //
            //             RowLayout {
            //                 anchors.fill: parent
            //                 anchors.leftMargin: 10
            //                 anchors.rightMargin: 10
            //                 spacing: 10
            //
            //                 Rectangle {
            //                     implicitWidth: 6
            //                     implicitHeight: 6
            //                     radius: 3
            //                     color: app_theme.online
            //                 }
            //
            //                 Text {
            //                     text: user_row.name
            //                     textFormat: Text.PlainText
            //                     color: app_theme.text
            //                     font.pixelSize: 14
            //                     elide: Text.ElideRight
            //                     Layout.fillWidth: true
            //                 }
            //             }
            //         }
            //
            //         ScrollBar.vertical: ScrollBar {
            //             policy: ScrollBar.AsNeeded
            //             palette.mid: app_theme.muted
            //             palette.dark: app_theme.muted
            //         }
            //     }
            // }
        }
    }
}
