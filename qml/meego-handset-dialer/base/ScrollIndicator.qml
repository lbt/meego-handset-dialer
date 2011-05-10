/*
 * Generic Scroll Indicator
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

    property Flickable target
    property color color: style.foreground

    property real showOpacity: 0.7
    property real hideOpacity: 0.0

    function show() {if(target.contentHeight > target.height) opacity = showOpacity}
    function hide() {if(!target.moving) opacity = hideOpacity}

    function autoHide() {autoHideTimer.start()}

    Component.onCompleted: {
        target.movementStarted.connect(root.show);
        target.movementEnded.connect(root.autoHide);
    }

    z: target.z + 1
    width: 10
    anchors {top: target.top; bottom: target.bottom; right: target.right; rightMargin: 4}
    opacity: hideOpacity

    Behavior on opacity {PropertyAnimation {duration: 750}}

    Timer {id: autoHideTimer; interval: 2000; repeat: false; onTriggered: hide()}

    Rectangle
    {
        id: bar
        y: target.visibleArea.yPosition * parent.height
        width: parent.width; height: target.visibleArea.heightRatio * parent.height
        color: parent.color
    }
}
