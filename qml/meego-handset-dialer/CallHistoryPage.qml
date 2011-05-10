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

    ListView
    {
        anchors {fill: parent; margins: 5}
        clip: true
        spacing: 5

        model: History

        delegate: CallHistoryItem {}

        ScrollIndicator {target: parent}
    }
}
