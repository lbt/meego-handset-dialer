/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "common.h"
#include "notificationdialog.h"
#include "dialerapplication.h"
#include <QDebug>

#include <MWidgetCreator>
M_REGISTER_WIDGET(AlertDialog)

NotificationDialog::NotificationDialog()
    : MDialog(qtTrId("xx_notification_title"), M::NoStandardButton),
      m_panel(new MStylableWidget()),
      m_layout(new MLayout(m_panel)),
      m_land(new MGridLayoutPolicy(m_layout)),
      m_port(new MGridLayoutPolicy(m_layout)),
      //% "Ok"
      m_ok(new MButton(qtTrId("xx_ok"))),
      m_message(new MLabel())
{
    TRACE
    // FIXME: Setting WindowModal is creating corrupted windows and
    //        occasionally segfaulting in GL...?
    //setWindowModal(true);
    setCloseButtonVisible(false);
    init();
    connect(this, SIGNAL(appeared()), SLOT(hideKeypad()));
    connect(this, SIGNAL(disappeared()), SLOT(showKeypad()));
}

NotificationDialog::~NotificationDialog()
{
    TRACE
}

void NotificationDialog::setNotification(const QString message)
{
    TRACE

    m_message->setText(message);
}

void NotificationDialog::init()
{
    TRACE

    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool);

    m_panel->setObjectName("notificationPanel");
    m_message->setObjectName("notificationMessage");

    connect(m_ok, SIGNAL(clicked()), SLOT(okPressed()));
    addButton(m_ok->model());

    m_layout->setLandscapePolicy(m_land);
    m_land->addItem(m_message, 0, 0, Qt::AlignCenter);

    m_layout->setPortraitPolicy(m_port);
    m_port->addItem(m_message, 0, 0, Qt::AlignCenter);

    setCentralWidget(m_panel);
}

void NotificationDialog::okPressed()
{
    TRACE

    disappear();
}

void NotificationDialog::cleanupOnDismiss()
{
    TRACE
}

void NotificationDialog::hideKeypad()
{
    TRACE

    DialerKeyPad *k = dynamic_cast<MainWindow *>(MApplication::instance()->activeApplicationWindow())->keypad();

    if (currentPageIs(GenericPage::PAGE_DIALER)) {
        if (!k->isOpen())
            k->open();
        k->disappear();
    }
}

void NotificationDialog::showKeypad()
{
    TRACE

    DialerKeyPad *k = dynamic_cast<MainWindow *>(MApplication::instance()->activeApplicationWindow())->keypad();
    if (currentPageIs(GenericPage::PAGE_DIALER))
	k->appear();
}
