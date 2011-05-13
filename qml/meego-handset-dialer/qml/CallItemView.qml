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

Item
{
  id: root
  width: parent.width * 0.9; height: parent.height

  property QtObject call

  state: 'disconnected'

  onCallChanged: {
    console.log("*** QML *** :: CALL ITEM CHANGED, STATE: " + call.state);
    root.state = call.state;

    call.stateChanged.connect(function(state) {
      console.log("*** QML *** :: CALL ITEM STATE CHANGED: " + state);
      console.log("*** QML *** ::    MSISDN: " + call.msisdn);
      console.log("");
      root.state = state;
    });
  }

  states {
    State {
      name: 'active'
      PropertyChanges {target: root; visible: true}
    }

    State {
      name: 'held'
      PropertyChanges {target: root; visible: true}
    }

    State {
      name: 'dialing'
      PropertyChanges {target: root; visible: true}
    }

    State {
      name: 'alerting'
      PropertyChanges {target: root; visible: true}
    }

    State {
      name: 'incoming'
      PropertyChanges {target: root; visible: true}
    }

    State {
      name: 'waiting'
      PropertyChanges {target: root; visible: true}
    }

    State {
      name: 'disconnected'
      PropertyChanges {target: root; visible: false}
    }
  }

  Rectangle
  {
    id: background
    anchors {fill: parent; margins: 10}
    radius: 15
    smooth: true

    gradient: Gradient {
      GradientStop {position: 0.0; color: '#4f4f4f'}
      GradientStop {position: 1.0; color: '#000000'}
    }
  }

  CallItemViewLarge
  {
    id: largeView
    call: parent.call
    state: parent.state
  }
}

