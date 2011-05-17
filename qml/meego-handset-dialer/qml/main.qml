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
import com.meego.dialer 1.0

import 'base'
import 'javascripts/framework.js' as Support

Item
{
    id: main

    width: 480; height: 800

    Dialer { id: adapter } // TODO: Refactor

    ContactModel {
        id: contactModel

        autoUpdate: true

        filter: DetailFilter {
            detail: ContactDetail.PhoneNumber
            field: PhoneNumber.Number
        }
    }

    Component.onCompleted: {
      console.log("######## Completed loading component, initializing...");

      adapter.incomingCall.connect(function()
      {
        var call = adapter.currentCall;

        console.log("*** QML *** :: INCOMING CALL:" + call);
        console.log("*** QML *** ::   MSISDN: " + call.msisdn);
        console.log("*** QML *** ::    START: " + call.startedAt);
        console.log("");

        dialpage.activeCall = call
        main.jumpTo(0);
        controller.show();
      });

      controller.setAdapter(adapter);

      if(adapter.currentCall)
      {
        dialpage.activeCall = call
        controller.show();
      }

      main.jumpTo(0);
    }

    function showErrorMessage(mesg) {
        mesgDialog.mesg = mesg;
        mesgDialog.state = 'shown';
    }

    function getContactByPhoneNumber(number)
    {
        var result = null;

        for(var i = 0; i < contactModel.contacts.length; i++)
        {
            var contact = contactModel.contacts[i];

            for(var j = 0; i < contact.phoneNumbers.length; j++)
            {
                if(contact.phoneNumbers[i].number.substr(-10) == number.substr(-10))
                {
                    result = contact;
                    break;
                }
            }

            if(result) break;
        }

        return result;
    }

    function dial(msisdn) {
        if(msisdn.trim().length == 0)
        {
            console.log("*** QML *** :: You can't dial without a number!");
            showErrorMessage("No number specified!");
            return false;
        }

        console.log('*** QML *** :: Attempting to dial MSISDN: ' + msisdn);
        adapter.dial(msisdn);

        dialpage.activeCall = {
          state: 'dialing',
          msisdn: msisdn
        };

        return true;
    }

    function jumpTo(idx) {
        switcher.jumpTo(idx);

        for(var i = 0; i < menubar.items.length; i++)
        {
            menubar.items[i].selected = (i == idx);
        }
    }

    function switchTo(idx)
    {
        switcher.switchTo(idx);
        for(var i = 0; i < menubar.items.length; i++)
        {
            menubar.items[i].selected = (i == idx);
        }
    }

    MeeGoTouchStyle {id: style}

    Rectangle {anchors.fill: parent; color: style.background}

    Menubar
    {
        id: menubar

        width: parent.width; height: 72

        items
        {
            MenubarButton {
                width: 70;
                icon: style.icon('icon-m-common-phone')
                onClicked: {main.switchTo(0)}
            }
            MenubarButton {
                width: 70;
                icon: style.icon('icon-m-common-clock')
                onClicked: {main.switchTo(1)}
            }
            MenubarButton {
                width: 70;
                icon: style.icon('icon-m-telephony-addressbook')
                onClicked: {main.switchTo(2)}
            }
            MenubarButton {
                width: 70;
                icon: style.icon('icon-m-content-favourites')
                onClicked: {main.switchTo(3)}
            }
        }
    }

    PageSwitcher
    {
        id: switcher

        width: parent.width
        anchors {top: menubar.bottom; bottom: parent.bottom}

        pages
        {
            DialPage {id: dialpage}
            CallHistoryPage {id: historypage}
            ContactsPage {id: contactspage}
            FavouritesPage {id: favouritespage}
        }
    }

    CallContextDialog {
        id: callContextDialog
        detail: '!! NAME'
        number: '!! NUMBER'
        state: 'hidden'
    }

    MessageDialog {
        id: mesgDialog
        state: 'hidden'
    }
}

