/*
 * MeeGo Touch Menubar
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

    property alias items: tools.children

    Rectangle
    {
        id: bg
        anchors.fill: parent
        color: style.windowTitleBackground
    }

    MenubarButton
    {
        id: bHome
        anchors {left: root.left; verticalCenter: parent.verticalCenter}
        icon: style.icon('icon-m-framework-home')
        background: bg.color
        onClicked: controller.showMinimized();
    }

    MenubarButton
    {
        id: bQuit
        anchors {right: root.right; verticalCenter: parent.verticalCenter}
        icon: style.icon('icon-m-framework-close')
        background: bg.color
        onClicked: controller.hide();
    }

    Row
    {
        id: tools
        anchors {horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter}
    }
}
