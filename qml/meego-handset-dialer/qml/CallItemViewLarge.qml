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

  anchors.fill: parent

  property variant call
  property string  callerLabelText: ''

  onCallChanged: {
    if(call && call.msisdn)
      {
        var contact = main.getContactByPhoneNumber(call.msisdn);
        if(contact){
            largeView.callerLabelText = contact.displayLabel;
        } else if (call.name) {
            largeView.callerLabelText = call.name;
        } else {
            largeView.callerLabelText = call.msisdn;
        }
      }
  }

  states {
    State {
      name: 'active'
      PropertyChanges {target: incomingTools; visible: false}
      PropertyChanges {target: answerButton; visible: false}
      PropertyChanges {target: hangupButton; visible: true}
      PropertyChanges {target: stateInd; text: 'Active'}
    }

    State {
      name: 'held'
      PropertyChanges {target: incomingTools; visible: false}
      PropertyChanges {target: answerButton; visible: false}
      PropertyChanges {target: hangupButton; visible: true}
      PropertyChanges {target: stateInd; text: 'Held'}
    }

    State {
      name: 'dialing'
      PropertyChanges {target: incomingTools; visible: false}
      PropertyChanges {target: answerButton; visible: false}
      PropertyChanges {target: hangupButton; visible: true}
      PropertyChanges {target: stateInd; text: 'Dialing...'}
    }

    State {
      name: 'alerting'
      PropertyChanges {target: incomingTools; visible: false}
      PropertyChanges {target: answerButton; visible: false}
      PropertyChanges {target: hangupButton; visible: true}
      PropertyChanges {target: stateInd; text: 'Alerting...'}
    }

    State {
      name: 'incoming'
      PropertyChanges {target: incomingTools; visible: true}
      PropertyChanges {target: answerButton; visible: true}
      PropertyChanges {target: hangupButton; visible: false}
      PropertyChanges {target: stateInd; text: 'Incoming...'}
    }

    State {
      name: 'waiting'
      PropertyChanges {target: incomingTools; visible: false}
      PropertyChanges {target: answerButton; visible: false}
      PropertyChanges {target: hangupButton; visible: true}
      PropertyChanges {target: stateInd; text: 'Waiting...'}
    }

    State {
      name: 'disconnected'
      PropertyChanges {target: incomingTools; visible: false}
      PropertyChanges {target: answerButton; visible: false}
      PropertyChanges {target: hangupButton; visible: false}
      PropertyChanges {target: stateInd; text: 'Disconnected'}
    }
  }

  Text
  {
    id: stateInd
    anchors {top: parent.top; topMargin: 20; horizontalCenter: parent.horizontalCenter}
    color: '#ffffff'
    font {pixelSize: 38}
  }

  Avatar
  {
    id: avatar
    width: 128; height: width
    anchors {horizontalCenter: parent.horizontalCenter; top: stateInd.bottom; topMargin: 20}
  }

  Text
  {
    id: callerInd
    anchors {top: avatar.bottom; topMargin: 20; horizontalCenter: parent.horizontalCenter}
    color: '#ffffff'
    font {pixelSize: 22}
    text: callerLabelText
  }

  Item
  {
    id: incomingTools
    height: 72

    anchors {bottom: answerButton.top; left: parent.left; right: parent.right; margins: 10; leftMargin: 24; rightMargin: 24}

    Rectangle
    {
      id: divertButton

      width: parent.width / 2 - 5; height: parent.height
      anchors {left: parent.left; verticalCenter: parent.verticalCenter}
      radius: 10
      color: '#ffffff'

      gradient: Gradient {
        GradientStop {position: 0.0; color: '#dfdfdf'}
        GradientStop {position: 1.0; color: '#4f4f4f'}
      }

      Image
      {
        width: 40; height: width
        anchors.centerIn: parent
        smooth: true
        source: '/usr/share/themes/base/meegotouch/icons/icon-m-telephony-call-diverted.svg'
      }

      MouseArea
      {
        anchors.fill: parent
        onClicked: {
          var divertTo = '';
          console.log('*** QML *** :: Diverting call to: ' + divertTo);
          root.call.deflect(divertTo);
        }
      }
    }

    Rectangle
    {
      width: divertButton.width; height: divertButton.height
      anchors {right: parent.right; verticalCenter: parent.verticalCenter}
      radius: 10

      gradient: Gradient {
        GradientStop {position: 0.0; color: '#dfdfdf'}
        GradientStop {position: 1.0; color: '#4f4f4f'}
      }

      Image
      {
        width: 40; height: width
        anchors.centerIn: parent
        smooth: true
        source: '/usr/share/themes/base/meegotouch/icons/icon-m-telephony-call-end.svg'
      }

      MouseArea
      {
        anchors.fill: parent
        onClicked: {
          console.log('*** QML *** :: Rejecting call');
          root.call.hangup();
        }
      }
    }
  }

  Rectangle
  {
    id: answerButton
    height: 72
    anchors {bottom: parent.bottom; left: parent.left; right: parent.right; margins: 20; leftMargin: 24; rightMargin: 24}

    radius: 10

    gradient: Gradient {
      GradientStop {position: 0.0; color: '#dfffdf'}
      GradientStop {position: 1.0; color: '#4f8f4f'}
    }

    Image
    {
      width: 40; height: width
      anchors.centerIn: parent
      smooth: true
      source: '/usr/share/themes/base/meegotouch/icons/icon-m-telephony-call-answer.svg'
    }

    MouseArea
    {
      anchors.fill: parent
      onClicked: {
        console.log('*** QML *** :: Answering call');
        root.call.answer();
      }
    }
  }

  Rectangle
  {
    id: hangupButton
    height: 72
    anchors {bottom: parent.bottom; left: parent.left; right: parent.right; margins: 20; leftMargin: 24; rightMargin: 24}

    radius: 10

    gradient: Gradient {
      GradientStop {position: 0.0; color: '#ffdfdf'}
      GradientStop {position: 1.0; color: '#8f4f4f'}
    }

    Image
    {
      width: 40; height: width
      anchors.centerIn: parent
      smooth: true
      source: '/usr/share/themes/base/meegotouch/icons/icon-m-telephony-call-end.svg'
    }

    MouseArea
    {
      anchors.fill: parent
      onClicked: {
        console.log('*** QML *** :: Hanging up call');
        root.call.hangup();
      }
    }
  }
}

