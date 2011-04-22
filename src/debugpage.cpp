/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "debugpage.h"
#include "managerproxy.h"
#include "dialerapplication.h"
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <QDebug>

#include <MWidgetCreator>
M_REGISTER_WIDGET(DebugPage)

DebugPage::DebugPage() :
    GenericPage(),
    mInfo(new MLabel()),
    mBox(new MContainer()),
    vmInfo(new MLabel()),
    vmBox(new MContainer()),
    nInfo(new MLabel()),
    nBox(new MContainer()),
    cInfo(new MLabel()),
    cBox(new MContainer())
{
    m_pageType = GenericPage::PAGE_DEBUG;
    setObjectName("debugPage");
    connect(this, SIGNAL(appeared()), SLOT(refreshContent()));
}

DebugPage::~DebugPage()
{
}

void DebugPage::activateWidgets()
{
    TRACE
    // Add any setup necessary for display of this page
    // then call the genericpage setup
    // Add your code here

    GenericPage::activateWidgets();
}

void DebugPage::deactivateAndResetWidgets()
{
    TRACE
    // Add any cleanup code for display of this page
    // then call the generic page cleanup
    // Add your code here

    GenericPage::deactivateAndResetWidgets();
}

void DebugPage::refreshContent()
{
    ManagerProxy *mp = ManagerProxy::instance();

    if ((mp != 0) && mp->isValid()) {
        // Modem
        if ((mp->modem() != 0) && mp->modem()->isValid()) {
            qDebug("refreshing modem info");
            mInfo->setText(QString(mp->modem()->dumpProperties().join("")));
            mBox->setText(mp->modem()->path());
        } else {
            //% "No modem available"
            mInfo->setText(qtTrId("xx_no_modem"));
            //% "None"
            mBox->setText(qtTrId("xx_none"));
        }

        // Voicemail
        if ((mp->voicemail() != 0) && mp->voicemail()->isValid()) {
            qDebug("refreshing voicemail info");
            vmInfo->setText(QString(mp->voicemail()->dumpProperties().join("")));
            vmBox->setText(mp->voicemail()->path());
        } else {
            //% "No MessageWaiting service available"
            vmInfo->setText(qtTrId("xx_no_messages_waiting"));
            //% "None"
            vmBox->setText(qtTrId("xx_none"));
        }

        // Network
        if ((mp->network() != 0) && mp->network()->isValid()) {
            qDebug("refreshing call info");
            nInfo->setText(mp->network()->dumpProperties().join(""));
            nBox->setText(mp->network()->name());
        } else {
            //% "Not registered with any network!"
            nInfo->setText(qtTrId("xx_not_registered"));
            //% "None"
            nBox->setText(qtTrId("xx_none"));
        }

        // Calls
        if ((mp->callManager() != 0) && mp->callManager()->isValid()) {
            qDebug("refreshing call info");
            cInfo->setText(mp->callManager()->dumpProperties().join(""));
        } else {
            //% "Not registered with any network!"
            cInfo->setText(qtTrId("xx_not_registered"));
        }
    } else {
        //% "No modem available"
        mInfo->setText(qtTrId("xx_no_modem"));
        //% "None"
        mBox->setText(qtTrId("xx_none"));
        //% "No MessageWaiting service available"
        vmInfo->setText(qtTrId("xx_no_messages"));
        //% "None"
        vmBox->setText(qtTrId("xx_none"));
        //% "No network available"
        nInfo->setText(qtTrId("xx_no_network"));
        //% "None"
        nBox->setText(qtTrId("xx_none"));
        //% "No calls in progress"
        cInfo->setText(qtTrId("xx_no_calls"));
    }
}

void DebugPage::createContent()
{
    GenericPage::createContent();

    // Modem
    //% "Modem:"
    mBox->setTitle(qtTrId("xx_modem"));
    //% "None"
    mBox->setText(qtTrId("xx_none"));
    mBox->setObjectName("debugModem");
    mBox->setIconID("small-mobile");
    //% "No modem available"
    mInfo->setText(qtTrId("xx_no_modem"));
    mInfo->setObjectName("debugLabel");
    mInfo->setAlignment(Qt::AlignTop);
    mInfo->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::MinimumExpanding));
    mBox->setCentralWidget(mInfo);

    // MessageWaiting
    //% "MessageWaiting:"
    vmBox->setTitle(qtTrId("xx_message_waiting"));
    //% "None"
    vmBox->setText(qtTrId("xx_none"));
    vmBox->setObjectName("debugModem");
    vmBox->setIconID("small-mobile");
    //% "No MessageWaiting service available"
    vmInfo->setText(qtTrId("xx_no_messages"));
    vmInfo->setObjectName("debugLabel");
    vmInfo->setAlignment(Qt::AlignTop);
    vmInfo->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::MinimumExpanding));
    vmBox->setCentralWidget(vmInfo);

    // Network
    //% "Network:"
    nBox->setTitle(qtTrId("xx_network"));
    //% "None"
    nBox->setText(qtTrId("xx_none"));
    nBox->setObjectName("debugNetwork");
    nBox->setIconID("small-home");
    //% "No network available"
    nInfo->setText(qtTrId("xx_no_network"));
    nInfo->setObjectName("debugLabel");
    nInfo->setAlignment(Qt::AlignTop);
    nInfo->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::MinimumExpanding));
    nBox->setCentralWidget(nInfo);

    // Calls
    //% "Calls:"
    cBox->setTitle(qtTrId("xx_calls"));
    cBox->setObjectName("debugNetwork");
    cBox->setIconID("icon-m-telephony-call");
    //% "No calls in progress"
    cInfo->setText(qtTrId("xx_no_calls"));
    cInfo->setObjectName("debugLabel");
    cInfo->setAlignment(Qt::AlignTop);
    cInfo->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::MinimumExpanding));
    cBox->setCentralWidget(cInfo);

    landscape->addItem(mBox, 0, 0, 1, 1, Qt::AlignTop|Qt::AlignLeft);
    landscape->addItem(vmBox,1, 0, 1, 1, Qt::AlignTop|Qt::AlignLeft);
    landscape->addItem(nBox, 0, 1, 1, 1, Qt::AlignTop|Qt::AlignLeft);
    landscape->addItem(cBox, 1, 1, 1, 1, Qt::AlignTop|Qt::AlignLeft);

    portrait->addItem(mBox,  0, 0, 1, 1, Qt::AlignTop|Qt::AlignLeft);
    portrait->addItem(vmBox, 1, 0, 1, 1, Qt::AlignTop|Qt::AlignLeft);
    portrait->addItem(nBox,  2, 0, 1, 1, Qt::AlignTop|Qt::AlignLeft);
    portrait->addItem(cBox,  3, 0, 1, 1, Qt::AlignTop|Qt::AlignLeft);

    refreshContent();
}

void DebugPage::toggleContainerVisible()
{
    qDebug() << "Header clicked";
    mBox->centralWidget()->setVisible(!mBox->centralWidget()->isVisible());
}
