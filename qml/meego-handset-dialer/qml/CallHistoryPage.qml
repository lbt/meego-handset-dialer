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
import QtMobility.contacts 1.2

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

        delegate: CallHistoryItem {
            contact: {
                var contact = main.getContactByPhoneNumber(model.LineID);
                return (contact ? contact.displayLabel : '');
            }
        }

        ScrollIndicator {target: parent}
    }
}

