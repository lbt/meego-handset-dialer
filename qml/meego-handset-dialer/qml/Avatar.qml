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

  property string image: '/usr/share/themes/base/meegotouch/icons/icon-m-telephony-avatar-placeholder.svg'

  states {
    State {
      name: 'small'
    }
    State {
      name: 'medium'
    }
    State {
      name: 'large'
    }
  }

  Rectangle
  {
    id: background
    anchors.fill: parent
    radius: 10
    color: '#ffffff'
    border {width: 1; color: '#000000'}
  }

  Image
  {
    id: image
    width: background.width * 0.8; height: width
    anchors.centerIn: parent
    smooth: true
    source: parent.image
  }
}

