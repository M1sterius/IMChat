import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    required property var theme
    readonly property int usersCount: users_list.count

    color: theme.surface
    radius: 10
    border.color: theme.border
    Layout.preferredHeight: 90
    Layout.fillWidth: true
    Layout.fillHeight: true

    ListView {
        id: users_list

        anchors.fill: parent
        anchors.margins: 8
        spacing: 4

        clip: true
        model: usersListModel

        delegate: Rectangle {
            id: user_row

            required property string name

            width: Math.max(0, users_list.width - 12)
            height: 40
            radius: 6
            color: user_hover.hovered ? theme.subtle : "transparent"

            HoverHandler {
                id: user_hover
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 10

                Rectangle {
                    implicitWidth: 6
                    implicitHeight: 6
                    radius: 3
                    color: theme.online
                }

                Text {
                    text: user_row.name
                    textFormat: Text.PlainText
                    color: theme.text
                    font.pixelSize: 14
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
            palette.mid: theme.muted
            palette.dark: theme.muted
        }
    }
}