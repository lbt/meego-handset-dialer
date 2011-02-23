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
#include "peopleitem.h"
#include "dialerpage.h"
#include "managerproxy.h"
#include "mainwindow.h"
#include "dialerkeypad.h"
#include "callitem.h"
#include "callitemmodel.h"
#include "dialerapplication.h"
#include "seasidesyncmodel.h"
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MTextEdit>
#include <MLabel>
#include <MImageWidget>
#include <MContentItem>
#include <MList>
#include <QDebug>
#include <QTextCursor>

#include <MWidgetCreator>
M_REGISTER_WIDGET(DialerPage)

#define MATCH_ANY(p) QRegExp(p,Qt::CaseInsensitive,QRegExp::FixedString)
#define MATCH_ALL QRegExp()

void DialerPage::pageShown()
{
    TRACE
    mainWindow()->keypad()->setTarget(m_entry);
    mainWindow()->keypad()->appear();
}

void DialerPage::pageHidden()
{
    TRACE
    mainWindow()->keypad()->disappear();
}

DialerPage::DialerPage() :
    GenericPage(),
    m_entry(new MTextEdit()),
    m_incall(false),
    m_layout(0),
    m_policy(0),
    //% "<-"
    m_bksp(new MButton("icon-m-common-backspace",qtTrId("xx_backspace"))),
    m_pressed(false),
    m_tapnhold(this)
{
    TRACE
    m_pageType = GenericPage::PAGE_DIALER;
    setObjectName("DialerPage");

    m_layout = new MLayout();
    m_policy = new MLinearLayoutPolicy(m_layout, Qt::Vertical);
    m_layout->setPolicy(m_policy);

    m_bksp->setTextVisible(false);
}

DialerPage::~DialerPage()
{
    TRACE
}

void DialerPage::createContent()
{
    TRACE
    GenericPage::createContent();

    // Widget creation

    // Widget properties
    m_entry->setObjectName("dialerEntryLabel");
    //% "Enter Number"
    m_entry->setPrompt(qtTrId("xx_number_prompt"));
    m_entry->setFocusPolicy(Qt::NoFocus);
    m_entry->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                       QSizePolicy::Expanding));
    m_bksp->setObjectName("dialerBkspButton");
    m_bksp->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                                       QSizePolicy::Expanding));

    MStylableWidget *foo = new MStylableWidget();
    foo->setObjectName("dialerActiveCall");
    foo->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                       QSizePolicy::MinimumExpanding));

    MStylableWidget *bar = new MStylableWidget();
    bar->setObjectName("dialerInactiveCall");
    bar->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                       QSizePolicy::MinimumExpanding));

    // Widget layout
    m_policy->setContentsMargins(0, -1, 0, -1);
    m_policy->insertItem(0, foo, Qt::AlignCenter);
    m_policy->addItem(bar, Qt::AlignCenter);
    landscape->addItem(m_entry,  0, 0, 1, 1, Qt::AlignLeft);
    landscape->addItem(m_bksp,   0, 1, 1, 1, Qt::AlignRight|Qt::AlignVCenter);
    landscape->addItem(m_layout, 1, 0, 1, 2, Qt::AlignCenter);

    portrait->addItem(m_entry,  0, 0, 1, 1, Qt::AlignLeft);
    portrait->addItem(m_bksp,   0, 1, 1, 1, Qt::AlignRight|Qt::AlignVCenter);
    portrait->addItem(m_layout, 1, 0, 1, 2, Qt::AlignCenter);

    // Set the Keypad Target when this window is shown
    connect(this, SIGNAL(appeared()), SLOT(pageShown()));
    connect(this, SIGNAL(disappeared()), SLOT(pageHidden()));

    CallManager *cm = ManagerProxy::instance()->callManager();
    if (cm->isValid())
        connect(cm, SIGNAL(callsChanged()), this, SLOT(updateCalls()));
    else
        qCritical("DialerPage: CallManager not ready yet!");

    // Hook up backspace key
    m_tapnhold.setSingleShot(true);
    connect(m_bksp, SIGNAL(pressed()), SLOT(handleBkspPress()));
    connect(m_bksp, SIGNAL(released()), SLOT(handleBkspRelease()));
}

void DialerPage::activateWidgets()
{
    TRACE
    // Add any setup necessary for display of this page
    // then call the genericpage setup
    // Add your code here

    GenericPage::activateWidgets();
}

void DialerPage::deactivateAndResetWidgets()
{
    TRACE
    // Add any cleanup code for display of this page
    // then call the generic page cleanup
    // Add your code here

    GenericPage::deactivateAndResetWidgets();
}

void DialerPage::updateCall(CallItem *call)
{
    // FIXME: Need to do something different with multiparty calls

    if (!call || call->peopleItem())
        return;

    QString lineid = call->lineID();
    //% "Unknown Caller"
    QString name = qtTrId("xx_unknown");
    QString photo  = DEFAULT_AVATAR_ICON;
    SeasideSyncModel *contacts = DA_SEASIDEMODEL;
    QModelIndex first = contacts->index(0,Seaside::ColumnPhoneNumbers);
    QModelIndexList matches = contacts->match(first, Seaside::SearchRole,
                                              QVariant(lineid),1);
    if (!matches.isEmpty()) {
        QModelIndex match = matches.at(0); //First match wins
        SEASIDE_SHORTCUTS
        SEASIDE_SET_MODEL_AND_ROW(match.model(), match.row());

        QString firstName = SEASIDE_FIELD(FirstName, String);
        QString lastName = SEASIDE_FIELD(LastName, String);

        if (lastName.isEmpty())
            // Contacts first (common) name
            //% "%1"
            name = qtTrId("xx_first_name").arg(firstName);
        else
            // BMC# 8079 - NW
            if (firstName.isEmpty())
               // Contacts last (sur) name
               //% "%1"
               name = qtTrId("xx_last_name").arg(lastName);
            else
               // Contacts full, sortable name, is "Firstname Lastname"
               //% "%1 %2"
               name = qtTrId("xx_first_last_name").arg(firstName)
                                                  .arg(lastName);

        photo = SEASIDE_FIELD(Avatar, String);
    }

    PeopleItem *person = new PeopleItem();
    person->setName(name);
    person->setPhoto(photo);
    person->setPhone(lineid);

    call->setPeopleItem(person);
    doClear();
}

void DialerPage::updateCalls()
{
    CallManager *cm = dynamic_cast<CallManager *>(sender());
    CallItem *activeCall = cm->activeCall();
    CallItem *heldCall = cm->heldCall();
    CallItem *incomingCall = cm->incomingCall();
    CallItem *waitingCall = cm->waitingCall();
    CallItem *dialingCall = cm->dialingCall();

    if (activeCall && !activeCall->peopleItem())
        updateCall(activeCall);

    if (heldCall && !heldCall->peopleItem())
        updateCall(heldCall);

    if (incomingCall && !incomingCall->peopleItem())
        updateCall(incomingCall);

    if (waitingCall && !waitingCall->peopleItem())
        updateCall(waitingCall);

    if (dialingCall && !dialingCall->peopleItem())
        updateCall(dialingCall);

    // Make sure active call is on top of the list
    if (activeCall && (m_policy->indexOf(activeCall) < 0)) {
        m_policy->insertItem(0, activeCall, Qt::AlignCenter);
    } else if (dialingCall && (m_policy->indexOf(dialingCall) < 0)) {
        m_policy->insertItem(0, dialingCall, Qt::AlignCenter);
    } else if (incomingCall && (m_policy->indexOf(incomingCall) < 0)) {
        m_policy->insertItem(0, incomingCall, Qt::AlignCenter);
    } else if (waitingCall && (m_policy->indexOf(waitingCall) < 0)) {
        m_policy->insertItem(0, waitingCall, Qt::AlignCenter);
    }
}

void DialerPage::doClear()
{
    TRACE
    m_pressed = false;
    m_entry->clear();
}

void DialerPage::doBackspace()
{
    TRACE
    m_entry->textCursor().deletePreviousChar();
}

void DialerPage::handleBkspPress()
{
    TRACE
    m_pressed = true;
    m_tapnhold.start(500);
    connect(&m_tapnhold, SIGNAL(timeout()), this, SLOT(doClear()));
}

void DialerPage::handleBkspRelease()
{
    TRACE
    if (m_tapnhold.isActive())
        m_tapnhold.stop();

    if (m_pressed) {
        m_pressed = false;
        doBackspace();
    }
}
