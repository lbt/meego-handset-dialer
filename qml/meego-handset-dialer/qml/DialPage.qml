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

Page
{
  id: root

  property alias activeCall: activeCallView.call

  DialNumberEntry
  {
    id: numberEntry
    anchors {
      top: parent.top; bottom: numPad.top
      left: parent.left; right: parent.right
      margins: 10
    }
  }

  DialNumPad
  {
    id: numPad
    width: parent.width
    anchors {bottom: parent.bottom}
    entry: numberEntry
  }

  CallItemView
  {
    id: activeCallView

    anchors {
        horizontalCenter: parent.horizontalCenter
    }
  }
}

