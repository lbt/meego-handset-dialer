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

    property string         name: ''
    property string       avatar: ''
    property string organisation: ''
    property bool      favourite: false

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
            id: iAvatar
            source: style.icon('icon-m-telephony-avatar-placeholder')
        }

        Text
        {
            id: iContactName
            anchors {left: iAvatar.right; leftMargin: 5}
            text: name
            font {pixelSize: 24}
        }

        Text
        {
            id: iContactOrganisation
            anchors {top: iContactName.bottom; left: iContactName.left; leftMargin: 10}
            text: organisation
            font {pixelSize: 18}
        }

        Image
        {
            id: iFavourite
            anchors {top: parent.top; right: parent.right}
            source: {
                if(favourite)
                {
                    style.icon('icon-s-common-favorite-mark');
                }
                else
                {
                    style.icon('icon-s-common-favorite-unmark');
                }
            }
        }

        Row
        {
            id: iBearerList
            anchors {top: iFavourite.bottom; topMargin: 10; right: parent.right}
            spacing: 8

            Image
            {
                width: 24
                smooth: true
                fillMode: Image.PreserveAspectFit
                source: style.icon('icon-m-telephony-cellular')
            }
            Image
            {
                width: 24
                smooth: true
                fillMode: Image.PreserveAspectFit
                source: style.icon('icon-m-telephony-ovi')
            }
            Image
            {
                width: 24
                smooth: true
                fillMode: Image.PreserveAspectFit
                source: style.icon('icon-m-telephony-skype')
            }
            Image
            {
                width: 24
                smooth: true
                fillMode: Image.PreserveAspectFit
                source: style.icon('icon-m-content-email')
            }
        }
    }

    MouseArea
    {
      anchors.fill: parent
      onClicked: {
        console.log("CLICKED!");
        for(var prop in model)
        {
          console.log(prop);
        }
      }
    }
}
