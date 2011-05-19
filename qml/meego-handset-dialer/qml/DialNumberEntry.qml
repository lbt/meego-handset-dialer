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
    property string placeHolderText: 'Enter Number'
    property TextInput textInput: input

    id: root

    function clear()
    {
        input.color = style.foregroundLight;
        input.text = placeHolderText;
    }

    function isBlank()
    {
        return (input.text == placeHolderText);
    }

    function appendChar(character)
    {
        if(input.text == placeHolderText) {input.text = character} else {input.text += character};
    }

    TextInput
    {
        id: input
        anchors {top: parent.top; left: parent.left; right: bksp.left; margins: 10}
        color: style.foregroundLight
        cursorVisible: false
        readOnly: true
        font {pixelSize: 42}
        text: placeHolderText
        onTextChanged: {
            if(text.length == 0) root.clear();

            if(text == placeHolderText)
            {
                color = style.foregroundLight
            }
            else
            {
                color = style.foreground;
            }
        }
    }

    Image
    {
        id: bksp
        anchors {top: parent.top; right: parent.right; margins: 24}
        source: style.icon('icon-m-common-backspace')
        MouseArea
        {
            anchors.fill: parent
            onClicked: {
                if(input.text == placeHolderText) return;
                input.text = input.text.substring(0, input.text.length - 1)
            }
            onPressAndHold: root.clear();
        }
    }
}
