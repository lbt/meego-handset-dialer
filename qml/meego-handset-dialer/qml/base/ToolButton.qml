/*
 * Generic Tool Button
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

    property alias        icon: image.source
    property alias      radius: bg.radius
    property alias      border: bg.border
    property color  background: style.buttonBackground

    signal clicked

    width: 72; height: 72

    SystemPalette {id: palette; colorGroup: SystemPalette.Active}

    Rectangle
    {
        id: bg
        anchors.fill: parent
        color: root.background
        radius: parent.radius
    }

    Image
    {
        id: image
        anchors {centerIn: parent}
        smooth: true
        fillMode: Image.PreserveAspectFit
    }

    MouseArea
    {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
