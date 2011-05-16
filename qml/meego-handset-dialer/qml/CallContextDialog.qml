/*
 * dialer - QML User Interface Component
 *
 * Copyright (c) 2011, Tom Swindell.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

import Qt 4.7
import 'base'

Item {
    id: root

    property string detail: ''
    property string number: ''

    anchors.fill: parent

    MeeGoTouchStyle {id: style}

    states: [
        State {
            name: 'shown'
            PropertyChanges {target: root; opacity: 1.0}
        },
        State {
            name: 'hidden'
            PropertyChanges {target: root; opacity: 0.0}
        }
    ]

    Behavior on opacity {PropertyAnimation {duration: 250}}

    Rectangle {
        id: blind
        anchors.fill: parent
        color: 'black'
        opacity: 0.7
    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            root.state = 'hidden'
        }
    }

    Rectangle {
        id: dialog
        width: parent.width * 0.8; height: 300;
        anchors.centerIn: parent
        color: style.background
        radius: 15
        smooth: true

        Text {
            id: titleText
            anchors {top: parent.top; horizontalCenter: parent.horizontalCenter; margins: 10}
            font.pixelSize: 46
            color: style.foreground
            text: 'Call?'
        }

        Image {
            id: avatarImage
            anchors {top: titleText.bottom; horizontalCenter: parent.horizontalCenter; margins: 10}
            source: style.icon('icon-m-telephony-avatar-placeholder')
        }

        Text {
            id: detailText
            anchors {top: avatarImage.bottom; horizontalCenter: parent.horizontalCenter; margins: 5}
            color: style.foreground
            text: detail
            font.pixelSize: 32
        }

        Text {
            id: numberText
            anchors {top: detailText.bottom; horizontalCenter: parent.horizontalCenter; margins: 5}
            color: style.foregroundLight
            text: number
            font.pixelSize: 32
        }

        ToolButton {
            id: callButton
            width: parent.width * 0.9; height: 72
            anchors {bottom: parent.bottom; horizontalCenter: parent.horizontalCenter; margins: 10}
            background: '#8fef8f'
            radius: 10
            icon: style.icon('icon-m-telephony-call')
            onClicked: {
                adapter.dial(number);
                root.state = 'hidden'
                main.switchTo(0);
            }
        }
    }
}

