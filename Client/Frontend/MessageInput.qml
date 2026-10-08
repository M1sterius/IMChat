import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

RowLayout {
    id: message_input

    required property var theme
    signal messageSent()

    spacing: 8

    function sendMessage() {
        const message = input_field.text.trim()

        if (message.length === 0)
            return

        chatService.onSendMessage(message)
        messageSent()
        input_field.clear()
        input_field.forceActiveFocus()
    }

    TextField {
        id: input_field

        Layout.fillWidth: true
        Layout.fillHeight: true

        placeholderText: "Enter your message..."
        maximumLength: 500
        wrapMode: TextInput.Wrap
        verticalAlignment: TextInput.AlignTop
        selectByMouse: true
        clip: true
        padding: 14
        color: message_input.theme.text
        placeholderTextColor: message_input.theme.muted
        selectionColor: message_input.theme.button
        selectedTextColor: message_input.theme.buttonText
        font.pixelSize: 14

        background: Rectangle {
            radius: 10
            color: message_input.theme.surface
            border.width: input_field.activeFocus ? 2 : 1
            border.color: input_field.activeFocus
                ? message_input.theme.accent : message_input.theme.border
        }

        onAccepted: message_input.sendMessage()
    }

    Button {
        id: send_button

        text: "Send"
        Layout.preferredWidth: 88
        Layout.fillHeight: true
        hoverEnabled: true

        enabled: input_field.text.trim().length > 0
        onClicked: message_input.sendMessage()

        contentItem: Text {
            text: send_button.text
            color: send_button.enabled ? message_input.theme.buttonText : message_input.theme.muted
            font.pixelSize: 14
            font.weight: Font.DemiBold
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            radius: 10
            color: !send_button.enabled ? message_input.theme.subtle
                : send_button.down ? message_input.theme.buttonPressed
                : send_button.hovered ? message_input.theme.buttonHover
                : message_input.theme.button
            border.width: send_button.visualFocus ? 2 : 1
            border.color: send_button.visualFocus
                ? message_input.theme.accent
                : send_button.enabled ? "transparent" : message_input.theme.border
        }
    }
}
