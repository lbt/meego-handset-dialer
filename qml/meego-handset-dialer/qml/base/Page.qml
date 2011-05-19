/*
 * Application Page
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

    property color backgroundColor: style.background

    width: parent.width; height: parent.height

    Behavior on x {PropertyAnimation {duration: 250}}

    Rectangle
    {
        id: bg
        anchors {fill: parent; margins: 2}
        color: root.backgroundColor
    }
}
