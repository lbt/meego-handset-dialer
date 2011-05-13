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

Item
{
    id: root

    property Item numpad

    property string text: ""
    property string detail: ""

    property color borderColor: style.buttonBorder
    property color backgroundColor: style.buttonBackground
    property color textColor: style.buttonText
    property color detailColor: style.buttonDetailText

    signal clicked
    signal pressAndHold

    width: parent.width / 3; height: 72

    SystemPalette {id: palette; colorGroup: SystemPalette.Active}

    Rectangle
    {
        anchors {fill: parent; margins: 2}
        color: root.backgroundColor;
        border {width: 1; color: root.borderColor}
        radius: width / 16
    }

    Text
    {
        width: parent.width
        height: parent.height * 0.6
        anchors {top: parent.top; margins: 4}
        text: parent.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font {pixelSize: parent.height / 2}
        color: parent.textColor
    }

    Text
    {
        width: parent.width
        height: parent.height / 4
        anchors {bottom: parent.bottom; margins: 8}
        text: parent.detail
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font {pixelSize: parent.height / 4}
        color: parent.detailColor
    }

    MouseArea
    {
        anchors.fill: parent

        onClicked: root.clicked();
        onPressAndHold: root.pressAndHold();
    }
}
