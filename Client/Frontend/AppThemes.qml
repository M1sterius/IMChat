import QtQuick

QtObject {
    property int paletteIndex: 0
    property int appearanceIndex: 0

    readonly property var paletteNames: [
        qsTr("Graphite & teal"),
        qsTr("Stone & olive"),
        qsTr("Ink & blue"),
        qsTr("Original peach & purple")
    ]
    readonly property var appearanceNames: [qsTr("System"), qsTr("Dark"), qsTr("Light")]

    readonly property bool lightMode: appearanceIndex === 2
        || (appearanceIndex === 0
            && Application.styleHints.colorScheme === Qt.Light)

    readonly property var palettes: [
        {
            light: {
                background: "#F1F3F2",
                surface: "#FFFFFF",
                subtle: "#E7ECEA",
                border: "#D5DCDA",
                text: "#202826",
                muted: "#596762",
                accent: "#28685D",
                button: "#28685D",
                buttonText: "#FFFFFF",
                buttonHover: "#21584F",
                buttonPressed: "#19483F",
                online: "#39784F"
            },
            dark: {
                background: "#171B1A",
                surface: "#202624",
                subtle: "#2B3330",
                border: "#3D4943",
                text: "#E7ECE9",
                muted: "#A5B3AB",
                accent: "#8DC7B5",
                button: "#8DC7B5",
                buttonText: "#15251F",
                buttonHover: "#A3D4C4",
                buttonPressed: "#78B6A3",
                online: "#8CC69D"
            }
        },
        {
            light: {
                background: "#F1F0EB",
                surface: "#FBFAF7",
                subtle: "#E9E8DF",
                border: "#D6D5CA",
                text: "#292B25",
                muted: "#676A5D",
                accent: "#59663D",
                button: "#59663D",
                buttonText: "#FFFFFF",
                buttonHover: "#495632",
                buttonPressed: "#3D482A",
                online: "#557347"
            },
            dark: {
                background: "#1C1D19",
                surface: "#252720",
                subtle: "#303329",
                border: "#44493A",
                text: "#E8E9DF",
                muted: "#B0B4A2",
                accent: "#BACB8C",
                button: "#BACB8C",
                buttonText: "#202714",
                buttonHover: "#CDDBA6",
                buttonPressed: "#A5B977",
                online: "#A9C58E"
            }
        },
        {
            light: {
                background: "#F1F3F5",
                surface: "#FFFFFF",
                subtle: "#E8EDF2",
                border: "#D5DCE3",
                text: "#202932",
                muted: "#5C6975",
                accent: "#365F89",
                button: "#365F89",
                buttonText: "#FFFFFF",
                buttonHover: "#2B5076",
                buttonPressed: "#234363",
                online: "#39774F"
            },
            dark: {
                background: "#191D22",
                surface: "#22282F",
                subtle: "#2C343E",
                border: "#414D5B",
                text: "#E7EDF3",
                muted: "#ACB8C5",
                accent: "#9DBFDF",
                button: "#9DBFDF",
                buttonText: "#182A3B",
                buttonHover: "#B2CDE7",
                buttonPressed: "#86AED3",
                online: "#8CC69D"
            }
        },
        {
            light: {
                background: "#f3dfcd",
                surface: "#ffe9d6",
                subtle: "#f4decb",
                border: "#d4bdad",
                text: "#3a3e5b",
                muted: "#706577",
                accent: "#3a3e5b",
                button: "#3a3e5b",
                buttonText: "#ffe9d6",
                buttonHover: "#484d6c",
                buttonPressed: "#30344d",
                online: "#008000"
            },
            dark: {
                background: "#30344d",
                surface: "#3a3e5b",
                subtle: "#444964",
                border: "#565a74",
                text: "#ffe9d6",
                muted: "#c5b9b3",
                accent: "#ffe9d6",
                button: "#ffe9d6",
                buttonText: "#3a3e5b",
                buttonHover: "#fff2e6",
                buttonPressed: "#ead1bc",
                online: "#008000"
            }
        }
    ]

    readonly property var colors: palettes[paletteIndex][lightMode ? "light" : "dark"]
    readonly property color background: colors.background
    readonly property color surface: colors.surface
    readonly property color subtle: colors.subtle
    readonly property color border: colors.border
    readonly property color text: colors.text
    readonly property color muted: colors.muted
    readonly property color accent: colors.accent
    readonly property color button: colors.button
    readonly property color buttonText: colors.buttonText
    readonly property color buttonHover: colors.buttonHover
    readonly property color buttonPressed: colors.buttonPressed
    readonly property color online: colors.online
    readonly property color scrim: Qt.rgba(0, 0, 0, lightMode ? 0.18 : 0.4)
}
