import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

Button {
    id: settings_button

    required property var theme

    text: "\u2699"
    implicitWidth: 44
    implicitHeight: 44
    hoverEnabled: true
    Accessible.name: qsTr("Settings")

    onClicked: settings_popup.opened ? settings_popup.close() : settings_popup.open()
    onVisibleChanged: {
        if (!visible)
            settings_popup.close()
    }

    ToolTip.visible: hovered && !settings_popup.opened
    ToolTip.delay: 600
    ToolTip.text: qsTr("Settings")

    contentItem: Text {
        text: settings_button.text
        color: settings_popup.opened
            ? settings_button.theme.buttonText : settings_button.theme.text
        font.pixelSize: 22
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        radius: 10
        color: settings_popup.opened ? settings_button.theme.button
            : settings_button.down || settings_button.hovered
                ? settings_button.theme.subtle : settings_button.theme.surface
        border.width: settings_popup.opened || settings_button.visualFocus ? 2 : 1
        border.color: settings_popup.opened || settings_button.visualFocus
            ? settings_button.theme.accent : settings_button.theme.border
    }

    component ChoiceButton: Button {
        id: choice_button

        required property bool selected

        Layout.fillWidth: true
        implicitHeight: 36
        hoverEnabled: true
        Accessible.role: Accessible.RadioButton
        Accessible.checkable: true
        Accessible.checked: selected

        contentItem: RowLayout {
            spacing: 10

            Rectangle {
                implicitWidth: 12
                implicitHeight: 12
                radius: 6
                color: "transparent"
                border.color: choice_button.selected
                    ? settings_button.theme.accent : settings_button.theme.muted

                Rectangle {
                    anchors.centerIn: parent
                    width: 6
                    height: 6
                    radius: 3
                    color: settings_button.theme.accent
                    visible: choice_button.selected
                }
            }

            Text {
                text: choice_button.text
                color: settings_button.theme.text
                font.pixelSize: 14
                font.weight: choice_button.selected ? Font.DemiBold : Font.Normal
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
        }

        background: Rectangle {
            radius: 6
            color: choice_button.down || choice_button.hovered || choice_button.selected
                ? settings_button.theme.subtle : "transparent"
            border.width: choice_button.visualFocus ? 2 : 0
            border.color: settings_button.theme.accent
        }
    }

    Popup {
        id: settings_popup

        parent: Overlay.overlay
        width: Math.min(280, parent.width - 24)
        height: Math.min(implicitHeight, parent.height - 24)
        x: Math.max(12, Math.min(
            settings_button.mapToItem(parent, 0, 0).x,
            parent.width - width - 12
        ))
        y: Math.max(12, Math.min(
            settings_button.mapToItem(parent, 0, settings_button.height + 8).y,
            parent.height - height - 12
        ))
        padding: 12
        focus: true
        modal: true
        dim: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        Overlay.modal: Rectangle {
            color: settings_button.theme.scrim
        }

        background: Rectangle {
            radius: 10
            color: settings_button.theme.surface
            border.width: 2
            border.color: settings_button.theme.accent
        }

        contentItem: Flickable {
            implicitHeight: settings_options.implicitHeight
            contentWidth: width
            contentHeight: settings_options.implicitHeight
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            ColumnLayout {
                id: settings_options

                width: parent.width
                spacing: 4

                Text {
                    text: qsTr("Settings")
                    color: settings_button.theme.text
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                    Layout.bottomMargin: 8
                }

                Text {
                    text: qsTr("Color palette")
                    color: settings_button.theme.muted
                    font.pixelSize: 12
                    Layout.bottomMargin: 4
                }

                Repeater {
                    model: settings_button.theme.paletteNames

                    delegate: ChoiceButton {
                        required property int index
                        required property string modelData

                        text: modelData
                        selected: settings_button.theme.paletteIndex === index
                        onClicked: settings_button.theme.paletteIndex = index
                    }
                }

                Rectangle {
                    color: settings_button.theme.border
                    implicitHeight: 1
                    Layout.fillWidth: true
                    Layout.topMargin: 8
                    Layout.bottomMargin: 8
                }

                Text {
                    text: qsTr("Appearance")
                    color: settings_button.theme.muted
                    font.pixelSize: 12
                    Layout.bottomMargin: 4
                }

                Repeater {
                    model: settings_button.theme.appearanceNames

                    delegate: ChoiceButton {
                        required property int index
                        required property string modelData

                        text: modelData
                        selected: settings_button.theme.appearanceIndex === index
                        onClicked: settings_button.theme.appearanceIndex = index
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
                palette.mid: settings_button.theme.muted
                palette.dark: settings_button.theme.muted
            }
        }
    }
}
