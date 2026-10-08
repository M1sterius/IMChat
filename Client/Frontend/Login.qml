import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

Popup {
    id: login_popup

    required property var theme
    required property var service

    parent: Overlay.overlay
    width: Math.min(360, parent.width - 24)
    height: Math.min(implicitHeight, parent.height - 24)
    anchors.centerIn: parent
    padding: 20
    z: 1
    modal: true
    dim: true
    focus: true
    visible: service.showLogin
    closePolicy: Popup.NoAutoClose

    function tryLogin() {
        if (!login_button.enabled)
            return

        service.onTryLogin(username_field.text.trim(), password_field.text, keep_login.checked)
    }

    onOpened: username_field.forceActiveFocus()
    onClosed: password_field.clear()

    Overlay.modal: Rectangle {
        color: login_popup.theme.background
    }

    background: Rectangle {
        radius: 10
        color: login_popup.theme.surface
        border.color: login_popup.theme.border
    }

    component LoginField: TextField {
        id: login_field

        Layout.fillWidth: true
        implicitHeight: 42
        selectByMouse: true
        clip: true
        padding: 12
        color: login_popup.theme.text
        placeholderTextColor: login_popup.theme.muted
        selectionColor: login_popup.theme.button
        selectedTextColor: login_popup.theme.buttonText
        font.pixelSize: 14

        background: Rectangle {
            radius: 10
            color: login_popup.theme.surface
            border.width: login_field.activeFocus ? 2 : 1
            border.color: login_field.activeFocus
                ? login_popup.theme.accent : login_popup.theme.border
        }
    }

    contentItem: Flickable {
        implicitHeight: login_form.implicitHeight
        contentWidth: width
        contentHeight: login_form.implicitHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ColumnLayout {
            id: login_form

            width: parent.width
            spacing: 12

            Text {
                text: qsTr("Login")
                color: login_popup.theme.text
                font.pixelSize: 16
                font.weight: Font.DemiBold
                Layout.bottomMargin: 4
            }

            Text {
                text: qsTr("Username")
                color: login_popup.theme.text
                font.pixelSize: 14
            }

            LoginField {
                id: username_field

                placeholderText: qsTr("Username")
                Accessible.name: qsTr("Username")
                onAccepted: password_field.forceActiveFocus()
            }

            Text {
                text: qsTr("Password")
                color: login_popup.theme.text
                font.pixelSize: 14
            }

            LoginField {
                id: password_field

                placeholderText: qsTr("Password")
                Accessible.name: qsTr("Password")
                echoMode: TextInput.Password
                onAccepted: login_popup.tryLogin()
            }

            CheckBox {
                id: keep_login

                text: qsTr("Keep me logged in")
                Layout.fillWidth: true
                implicitHeight: 32
                spacing: 10
                hoverEnabled: true

                indicator: Rectangle {
                    x: keep_login.leftPadding
                    y: (keep_login.height - height) / 2
                    implicitWidth: 20
                    implicitHeight: 20
                    radius: 6
                    color: keep_login.checked ? login_popup.theme.button
                        : keep_login.hovered ? login_popup.theme.subtle : login_popup.theme.surface
                    border.width: keep_login.visualFocus ? 2 : 1
                    border.color: keep_login.checked || keep_login.visualFocus
                        ? login_popup.theme.accent : login_popup.theme.border

                    Text {
                        anchors.centerIn: parent
                        text: "\u2713"
                        color: login_popup.theme.buttonText
                        font.pixelSize: 14
                        visible: keep_login.checked
                    }
                }

                contentItem: Text {
                    text: keep_login.text
                    color: login_popup.theme.text
                    font.pixelSize: 14
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: keep_login.indicator.width + keep_login.spacing
                }
            }

            Button {
                id: login_button

                text: qsTr("Login")
                Layout.fillWidth: true
                implicitHeight: 42
                hoverEnabled: true
                enabled: username_field.text.trim().length > 0 && password_field.text.length > 0
                onClicked: login_popup.tryLogin()

                contentItem: Text {
                    text: login_button.text
                    color: login_button.enabled ? login_popup.theme.buttonText : login_popup.theme.muted
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    radius: 10
                    color: !login_button.enabled ? login_popup.theme.subtle
                        : login_button.down ? login_popup.theme.buttonPressed
                        : login_button.hovered ? login_popup.theme.buttonHover
                        : login_popup.theme.button
                    border.width: login_button.visualFocus ? 2 : 1
                    border.color: login_button.visualFocus
                        ? login_popup.theme.accent
                        : login_button.enabled ? "transparent" : login_popup.theme.border
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
            palette.mid: login_popup.theme.muted
            palette.dark: login_popup.theme.muted
        }
    }
}
