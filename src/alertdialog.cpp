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
#include "alertdialog.h"
#include "dialerapplication.h"
#include <QDebug>

#include <MWidgetCreator>
M_REGISTER_WIDGET(AlertDialog)

AlertDialog::AlertDialog()
      //% "Incoming call from:"
    : MDialog(qtTrId("xx_incoming_call_title"), M::NoStandardButton),
      m_panel(new MStylableWidget()),
      m_layout(new MLayout(m_panel)),
      m_land(new MGridLayoutPolicy(m_layout)),
      m_port(new MGridLayoutPolicy(m_layout)),
      //% "Accept"
      m_accept(new MButton(qtTrId("xx_accept"))),
      //% "Decline"
      m_decline(new MButton(qtTrId("xx_decline"))),
      m_photo(new MImageWidget()),
      m_name(new MLabel()),
      m_number(new MLabel())
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

AlertDialog::~AlertDialog()
{
    TRACE
}

void AlertDialog::setCallItem(CallItem *call)
{
    TRACE
    m_call = call;
    updateInfo();
    connect(m_call, SIGNAL(stateChanged()), SLOT(callStateChanged()));
}

void AlertDialog::init()
{
    TRACE
    m_panel->setObjectName("alertPanel");
    m_photo->setObjectName("alertPhoto");
    m_name->setObjectName("alertName");
    m_number->setObjectName("alertNumber");
    m_accept->setObjectName("acceptButton");
    m_decline->setObjectName("declineButton");

    connect(m_accept, SIGNAL(clicked()), SLOT(acceptCall()));
    connect(m_decline, SIGNAL(clicked()), SLOT(declineCall()));
    addButton(m_accept->model());
    addButton(m_decline->model());

    m_layout->setLandscapePolicy(m_land);
    m_land->addItem(m_photo,  0, 0, 2, 1, Qt::AlignVCenter|Qt::AlignLeft);
    m_land->addItem(m_name,   0, 1,       Qt::AlignLeft);
    m_land->addItem(m_number, 1, 1,       Qt::AlignLeft);

    m_layout->setPortraitPolicy(m_port);
    m_port->addItem(m_photo,  0, 0, Qt::AlignCenter);
    m_port->addItem(m_name,   1, 0, Qt::AlignLeft);
    m_port->addItem(m_number, 2, 0, Qt::AlignLeft);

    setCentralWidget(m_panel);
}

void AlertDialog::acceptCall()
{
    TRACE
    if (!m_call || !m_call->isValid())
        return;
    disconnect(m_call, SIGNAL(stateChanged()));

    // A call is "waiting" if there is an active call already and a new
    // call comes in.  This is handled by the call manager object instead
    if (m_call->state() == CallItemModel::STATE_WAITING) {
        CallManager *cm = ManagerProxy::instance()->callManager();
        if (cm && cm->isValid())
            cm->holdAndAnswer();
        else
            qCritical("CallManager is invalid, cannot answer waiting calls");
    } else {
        m_call->callProxy()->answer();
    }

    // Switch to the dialer page so we can see the call just accepted
    DialerApplication *app = DialerApplication::instance();
    MainWindow *mw = dynamic_cast<MainWindow *>(app->activeApplicationWindow());
    mw->m_pages.at(GenericPage::PAGE_DIALER)->appear();
}

void AlertDialog::declineCall()
{
    TRACE
    if (!m_call || !m_call->isValid())
        return;
    disconnect(m_call, SIGNAL(stateChanged()));
    m_call->callProxy()->hangup();
}

void AlertDialog::callStateChanged()
{
    TRACE
    reject();
}

void AlertDialog::hideKeypad()
{
    TRACE
    DialerKeyPad *k = dynamic_cast<MainWindow *>(MApplication::instance()->activeApplicationWindow())->keypad();

    if (currentPageIs(GenericPage::PAGE_DIALER)) {
        if (!k->isOpen())
            k->open();
        k->disappear();
    }
}

void AlertDialog::showKeypad()
{
    TRACE
    DialerKeyPad *k = dynamic_cast<MainWindow *>(MApplication::instance()->activeApplicationWindow())->keypad();
    if (currentPageIs(GenericPage::PAGE_DIALER))
        k->appear();
}

void AlertDialog::updateInfo()
{
    TRACE

    //% "Unknown Caller"
    QString name   = qtTrId("xx_unknown_caller");
    QString photo  = "icon-m-content-avatar-placeholder";
    //% "Private"
    QString lineid = qtTrId("xx_private");

    if (m_call && m_call->isValid() && !m_call->lineID().isEmpty()) {
        lineid = stripLineID(m_call->lineID());
        SeasideSyncModel *contacts = DA_SEASIDEMODEL;
        QModelIndex first = contacts->index(0,Seaside::ColumnPhoneNumbers);
        QModelIndexList matches = contacts->match(first, Seaside::SearchRole,
                                                  QVariant(lineid),1);
        if (!matches.isEmpty()) {
            QModelIndex person = matches.at(0); //First match is all we look at
            SEASIDE_SHORTCUTS
            SEASIDE_SET_MODEL_AND_ROW(person.model(), person.row());

            // Contacts full, sortable name, defaults to "Lastname, Firstname"
            //% "%1, %2"
            name = qtTrId("xx_full_name").arg(SEASIDE_FIELD(LastName, String))
                                         .arg(SEASIDE_FIELD(FirstName, String));
            photo = SEASIDE_FIELD(Avatar, String);
        }
    } else {
        //% "Unavailable"
        lineid = qtTrId("xx_unavailable");
    }

    m_name->setText(name);
    m_photo->setImage(photo);
    m_number->setText(lineid);
}
