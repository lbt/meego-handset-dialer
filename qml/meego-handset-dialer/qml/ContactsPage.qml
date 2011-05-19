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

        model: ContactModel {
            autoUpdate: true

            filter: DetailFilter {
                detail: ContactDetail.PhoneNumber
                field: PhoneNumber.Number
            }

            sortOrders: [
                SortOrder {
                    detail: ContactDetail.Name
                    field: Name.FirstName
                    direction: Qt.AscendingOrder
                },
                SortOrder {
                    detail: ContactDetail.Name
                    field: Name.LastName
                    direction: Qt.AscendingOrder
                }
            ]
        }

        delegate: ContactItem {
            name: model.display
            number: model.contact.phoneNumber.number
        }

        ScrollIndicator {target: parent}
    }
}

