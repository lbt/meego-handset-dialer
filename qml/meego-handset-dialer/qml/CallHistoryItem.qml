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
import "javascripts/framework.js" as Support

Item
{
    id: root

    property string    avatar: ''
    property string   contact: model.display
    property string    number: ''

    property string    bearer: 'cellular'
    property string    direction: ''

    property string startedAt: ''
    property int     duration: 0

    width: parent.width - 1; height: contents.height + 20

    Rectangle
    {
        anchors.fill: parent
        radius: 5
        border {width: 1; color: '#000000'}
    }

    Item
    {
        id: contents

        width: parent.width - 20; height: childrenRect.height
        anchors.centerIn: parent

        Image
        {
            id: iBearer
            anchors {right: parent.right}
            source: style.icon('icon-m-telephony-' + bearer)
        }

        Image
        {
            id: iDirection
            width: 24
            anchors.verticalCenter: iAvatar.verticalCenter
            fillMode: Image.PreserveAspectFit
            smooth: true
            source: {
                if(direction == "CallItem.In")
                {
                    style.icon('icon-m-telephony-call-received')
                }
                else if(direction == "CallItem.Out")
                {
                    style.icon('icon-m-telephony-call-initiated')
                }
                else
                {
                    style.icon('icon-m-telephony-call-missed')
                }
            }
        }

        Image
        {
            id: iAvatar
            anchors {left: iDirection.right}
            source: style.icon('icon-m-telephony-avatar-placeholder')
        }

        Text
        {
            id: iContactName
            anchors {left: iAvatar.right; leftMargin: 5}
            text: contact
            font {pixelSize: 24}
        }

        Text
        {
            id: iContactNumber
            anchors {left: iContactName.left; top: iContactName.bottom}
            text: number
            font {pixelSize: 18}
        }

        Text
        {
            id: iStarted
            anchors {top: iAvatar.bottom; topMargin: 10; left: iAvatar.left}
            text: (new Date()).getFormat();
            font {pixelSize: 14}
        }

        Text
        {
            id: iDuration
            anchors {top: iStarted.top; right: parent.right}
            text: Support.friendlyDuration(duration)
            font: iStarted.font
        }
    }

    MouseArea
    {
        anchors.fill: parent
        onClicked: {
        }
        onPressAndHold: {
        }
    }
}
