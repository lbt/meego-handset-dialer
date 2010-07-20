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
            mInfo->setText("No modem available");
            mBox->setText("None");
        }

        // Voicemail
        if ((mp->voicemail() != 0) && mp->voicemail()->isValid()) {
            qDebug("refreshing voicemail info");
            vmInfo->setText(QString(mp->voicemail()->dumpProperties().join("")));
            vmBox->setText(mp->voicemail()->path());
        } else {
            vmInfo->setText("No MessageWaiting service available");
            vmBox->setText("None");
        }

        // Network
        if ((mp->network() != 0) && mp->network()->isValid()) {
            qDebug("refreshing call info");
            nInfo->setText(mp->network()->dumpProperties().join(""));
            nBox->setText(mp->network()->name());
        } else {
            nInfo->setText("Not registered with any network!");
            nBox->setText("None");
        }

        // Calls
        if ((mp->callManager() != 0) && mp->callManager()->isValid()) {
            qDebug("refreshing call info");
            cInfo->setText(mp->callManager()->dumpProperties().join(""));
        } else {
            cInfo->setText("Not registered with any network!");
        }
    } else {
        mInfo->setText("No modem available");
        mBox->setText("None");
        mInfo->setText("No MessageWaiting service available");
        mBox->setText("None");
        nInfo->setText("No network available");
        nBox->setText("None");
        cInfo->setText("No calls in progress");
    }
}

void DebugPage::createContent()
{
    GenericPage::createContent();

    // Modem
    mBox->setTitle("Modem:");
    mBox->setText("None");
    mBox->setObjectName("debugModem");
    mBox->setIconID("small-mobile");
    mInfo->setText("No modem available");
    mInfo->setObjectName("debugLabel");
    mInfo->setAlignment(Qt::AlignTop);
    mInfo->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::MinimumExpanding));
    mBox->setCentralWidget(mInfo);

    // MessageWaiting
    vmBox->setTitle("MessageWaiting:");
    vmBox->setText("None");
    vmBox->setObjectName("debugModem");
    vmBox->setIconID("small-mobile");
    vmInfo->setText("No MessageWaiting service available");
    vmInfo->setObjectName("debugLabel");
    vmInfo->setAlignment(Qt::AlignTop);
    vmInfo->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::MinimumExpanding));
    vmBox->setCentralWidget(vmInfo);

    // Network
    nBox->setTitle("Network:");
    nBox->setText("None");
    nBox->setObjectName("debugNetwork");
    nBox->setIconID("small-home");
    nInfo->setText("No network available");
    nInfo->setObjectName("debugLabel");
    nInfo->setAlignment(Qt::AlignTop);
    nInfo->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::MinimumExpanding));
    nBox->setCentralWidget(nInfo);

    // Calls
    cBox->setTitle("Calls:");
    cBox->setObjectName("debugNetwork");
    cBox->setIconID("icon-m-telephony-call");
    cInfo->setText("No calls in progress");
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
