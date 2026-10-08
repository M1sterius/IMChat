import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

Rectangle {
    id: history

    required property var theme
    readonly property int messageCount: message_list.count

    function scrollToLatest() {
        Qt.callLater(function() {
            message_list.forceLayout()
            message_list.positionViewAtEnd()
        })
    }

    Component.onCompleted: scrollToLatest()

    color: theme.surface
    radius: 10
    border.color: theme.border

    ListView {
        id: message_list

        anchors.fill: parent
        anchors.margins: 12

        clip: true
        model: messageHistoryModel
        spacing: 4

        // One delegate instance displays one message.
        delegate: Item {
            id: messageRow

            required property string author
            required property string timeLabel
            required property string body

            width: message_list.width
            height: message_content.implicitHeight + 16

            ColumnLayout {
                id: message_content

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 6
                anchors.rightMargin: 18
                anchors.top: parent.top
                anchors.topMargin: 8
                spacing: 4

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Text {
                        text: messageRow.author
                        textFormat: Text.PlainText
                        color: history.theme.accent
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                        elide: Text.ElideRight
                        Layout.maximumWidth: message_content.width * 0.7
                    }

                    Text {
                        text: messageRow.timeLabel
                        color: history.theme.muted
                        font.pixelSize: 11
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                Text {
                    text: messageRow.body
                    textFormat: Text.PlainText
                    wrapMode: Text.Wrap
                    color: history.theme.text
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }
            }
        }

        // Group adjacent messages by their day role.
        section.property: "day"
        section.criteria: ViewSection.FullString
        section.labelPositioning: ViewSection.InlineLabels

        section.delegate: Item {
            id: date_section

            required property string section

            width: message_list.width
            height: 44

            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                height: 1
                color: history.theme.border
            }

            Rectangle {
                anchors.centerIn: parent
                width: date_label.implicitWidth + 24
                height: 28
                radius: 14
                color: history.theme.surface

                Text {
                    id: date_label
                    anchors.centerIn: parent
                    text: Qt.formatDate(
                        Date.fromLocaleString(Qt.locale(), date_section.section, "yyyy-MM-dd"),
                        "d MMMM"
                    )
                    color: history.theme.muted
                    font.pixelSize: 11
                    font.weight: Font.Medium
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
            palette.mid: history.theme.muted
            palette.dark: history.theme.muted
        }
    }
}
