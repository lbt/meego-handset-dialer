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

    property bool numPadShown: true
    property DialNumberEntry entry

    height: 72 * 5 + 4

    function insertText(text)
    {
        entry.appendChar(text)
    }

    function show()
    {
        height = 72 * 5 + 4;
        numPadShown = true;
    }

    function hide()
    {
        height = 72;
        numPadShown = false;
    }

    function toggle()
    {
        if(numPadShown == true) {hide()} else {show()}
    }

    Behavior on height {PropertyAnimation {duration: 500; easing.type: Easing.OutBounce}}

    Item
    {
        id: numpad
        width: parent.width; height: 72 * 4

        Behavior on opacity {PropertyAnimation {duration: 500}}

        Column
        {
            anchors.fill: parent

            Row
            {
                width: parent.width - 10
                anchors.horizontalCenter: parent.horizontalCenter
                DialNumPadButton {
                    text: "1";
                    detail: "voicemail";
                    onClicked: root.insertText('1');
                    onPressAndHold: Dialer.dialVoiceMail();
                }
                DialNumPadButton {
                    text: "2";
                    detail: "abc";
                    onClicked: root.insertText('2');
                    onPressAndHold: Dialer.dialSpeedDial(2);
                }
                DialNumPadButton {
                    text: "3";
                    detail: "def";
                    onClicked: root.insertText('3');
                    onPressAndHold: Dialer.dialSpeedDial(3);
                }
            }
            Row
            {
                width: parent.width - 10
                anchors.horizontalCenter: parent.horizontalCenter
                DialNumPadButton {
                    text: "4";
                    detail: "ghi";
                    onClicked: root.insertText('4');
                    onPressAndHold: Dialer.dialSpeedDial(4);
                }
                DialNumPadButton {
                    text: "5";
                    detail: "jkl";
                    onClicked: root.insertText('5');
                    onPressAndHold: Dialer.dialSpeedDial(5);
                }
                DialNumPadButton {
                    text: "6";
                    detail: "mno";
                    onClicked: root.insertText('6');
                    onPressAndHold: Dialer.dialSpeedDial(6);
                }
            }
            Row
            {
                width: parent.width - 10
                anchors.horizontalCenter: parent.horizontalCenter
                DialNumPadButton {
                    text: "7";
                    detail: "pqrs";
                    onClicked: root.insertText('7');
                    onPressAndHold: Dialer.dialSpeedDial(7);
                }
                DialNumPadButton {
                    text: "8";
                    detail: "tuv";
                    onClicked: root.insertText('8');
                    onPressAndHold: Dialer.dialSpeedDial(8);
                }
                DialNumPadButton {
                    text: "9";
                    detail: "wxyz";
                    onClicked: root.insertText('9');
                    onPressAndHold: Dialer.dialSpeedDial(9);
                }
            }
            Row
            {
                width: parent.width - 10
                anchors.horizontalCenter: parent.horizontalCenter
                DialNumPadButton {
                    text: "*";
                    onClicked: root.insertText('*');
                    onPressAndHold: root.insertText('p');
                }
                DialNumPadButton {
                    text: "0";
                    detail: "+";
                    onClicked: root.insertText('0');
                    onPressAndHold: root.insertText('+');
                }
                DialNumPadButton {
                    text: "#";
                    onClicked: root.insertText('#');
                    onPressAndHold: root.insertText('w');
                }
            }
        }
    }

    Item
    {
        id: actions

        width: parent.width; height: 74
        anchors {bottom: parent.bottom}

        Rectangle
        {
            anchors.fill: parent
            color: style.background
        }

        Item
        {
            anchors {fill: parent; margins: 4}

            ToolButton {
                id: bAddContact;
                anchors {left: parent.left}
                width: parent.width / 4; height: parent.height
                radius: 10
                background: style.windowToolsNormal
                icon: style.icon('icon-m-common-add-contact')
            }
            ToolButton {
                id: bCall;
                anchors {left: bAddContact.right; right: bViewToggle.left; margins: 5}
                height: parent.height
                radius: 10
                icon: style.icon('icon-m-telephony-call');
                onClicked: {
                    if(entry.isBlank())
                    {
                        console.log('*** QML *** :: You can not dial without a number!');
                        main.showErrorMessage("You can't dial without a number!");
                        return;
                    }

                    if(main.dial(entry.textInput.text))
                    {
                        entry.clear();
                    }
                }
                background: '#8fef8f'
            }
            ToolButton {
                id: bViewToggle;
                anchors {right: parent.right}
                width: parent.width / 4; height: parent.height
                radius: 10
                background: style.windowToolsNormal
                icon: style.icon('icon-m-telephony-dialer');
                onClicked: root.toggle()
            }
        }
    }
}
