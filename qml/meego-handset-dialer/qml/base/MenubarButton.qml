/*
 * MeeGo Touch Menubar Button
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
    property color normalColor: style.windowToolsNormal
    property color highlightColor: style.windowToolsActive

    property alias background: bg.color
    property alias icon: image.source
    property bool selected: false

    signal clicked

    width: 68; height: width

    onSelectedChanged: {bg.color = selected == true ? highlightColor : normalColor}

    SystemPalette {id: palette; colorGroup: SystemPalette.Active}

    Rectangle
    {
        id: bg
        anchors {fill: parent; margins: 5}
        color: normalColor
        radius: 10
    }

    Image
    {
        id: image
        smooth: true
        fillMode: Image.PreserveAspectFit
        anchors {centerIn: parent; fill: parent; margins: 15}
    }

    MouseArea
    {
        anchors.fill: parent
        onClicked: parent.clicked()
    }
}
