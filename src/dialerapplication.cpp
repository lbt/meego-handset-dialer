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
#include "dbustypes.h"
#include "dialerapplication.h"
#include "dialerpage.h"
#include "peoplepage.h"
#include "recentpage.h"
#include "favoritespage.h"
#include "debugpage.h"
#include <QtGui>
#include <QDebug>
#include <QApplication>
#include <MApplication>
#include <MApplicationService>
#include <MApplicationWindow>
#include <MSceneManager>
#include <MGridLayoutPolicy>
#include <MTheme>
#include <MButton>
#include <MAction>
#include <MWidgetAction>
#include <MToolBar>
#include <MButtonGroup>
#include <MDialog>
#include <MNotification>

DialerApplication::DialerApplication(int &argc, char **argv)
    : MApplication(argc, argv)
{
    TRACE
    init();
}

DialerApplication::DialerApplication(int &argc, char **argv, MApplicationService *service)
    : MApplication(argc, argv, service)
{
    TRACE

    setPrestartMode(M::LazyShutdown);
    init();
}

void DialerApplication::releasePrestart()
{
    TRACE
    // Now is the time for set up and display of information
    // that needs to be done to allow the dialeror other
    // pages to display correctly when opened. GenericPage has a
    // activateWidgets() method for common setup and
    // each Page type (dialer, people, favorites, etc) can also
    // implement the method for Page specific setup and signal
    // and slot connections

    MApplication::releasePrestart();
}

void DialerApplication::restorePrestart()
{
    TRACE
    // Now is the time for clean up and resetting an information
    // that needs to be done to allow the dialer pages to display
    // correctly when reopened. GenericPage has a
    // deactivateAndResetWidgets() method for common setup and
    // each Page type (dialer, people, favorites, etc) can also
    // implement the method for Page specific clean up

    // Call the default implementation to hide the window.
    MApplication::restorePrestart();
}

void DialerApplication::connectAll()
{
    TRACE

    connect(m_manager->modem(), SIGNAL(connected()),
                                SLOT(modemConnected()));
    connect(m_manager->modem(), SIGNAL(disconnected()),
                                SLOT(modemDisconnected()));
    connect(m_manager->network(), SIGNAL(connected()),
                                  SLOT(networkConnected()));
    connect(m_manager->network(), SIGNAL(disconnected()),
                                  SLOT(networkDisconnected()));
    connect(m_manager->callManager(), SIGNAL(connected()),
                                      SLOT(callManagerConnected()));
    connect(m_manager->callManager(), SIGNAL(disconnected()),
                                      SLOT(callManagerDisconnected()));
    connect(m_manager->callManager(), SIGNAL(callsChanged()),
                                      SLOT(onCallsChanged()));
    connect(m_manager->voicemail(), SIGNAL(messagesWaitingChanged()),
                                      SLOT(messagesWaitingChanged()));
}

bool DialerApplication::isConnected()
{
    TRACE
    return m_connected;
}

bool DialerApplication::hasError() const
{
    TRACE
    return !m_lastError.isEmpty();
}

QString DialerApplication::lastError() const
{
    TRACE
    return m_lastError;
}

void DialerApplication::setError(const QString msg)
{
    TRACE
    m_lastError.clear();
    m_lastError = QString(msg);
}

DialerApplication *DialerApplication::instance()
{
    TRACE
    return qobject_cast<DialerApplication *>(MApplication::instance());
}

SeasideSyncModel *DialerApplication::seasideModel()
{
    TRACE
    return m_seasideModel;
}

SeasideProxyModel *DialerApplication::seasideProxy()
{
    TRACE
    return m_seasideProxy;
}

HistoryTableModel *DialerApplication::historyModel()
{
    TRACE
    return m_historyModel;
}

QSortFilterProxyModel *DialerApplication::historyProxy()
{
    TRACE
    return m_historyProxy;
}

void DialerApplication::init()
{
    TRACE
    m_connected = false;
    m_lastError = QString();

    // Notify Qt of our custom DBus MetaTypes
    registerMyDataTypes();

    m_manager = ManagerProxy::instance();
    if (!m_manager || !m_manager->isValid())
        //% "Failed to connect to org.ofono.Manager: is ofonod running?"
        setError(qtTrId("xx_no_ofono_error"));
    else
        m_connected = true;

    m_seasideModel = new SeasideSyncModel();
    m_seasideProxy = new SeasideProxyModel();
    m_seasideProxy->setSourceModel(m_seasideModel);
    m_seasideProxy->setDynamicSortFilter(true);
    m_seasideProxy->setFilterKeyColumn(-1);
    m_seasideProxy->setFilterRegExp(MATCH_ALL);
    m_seasideProxy->sort(Seaside::ColumnLastName, Qt::AscendingOrder);

    m_historyModel = new HistoryTableModel();
    m_historyProxy = new QSortFilterProxyModel();
    m_historyProxy->setSourceModel(m_historyModel);
    m_historyProxy->setDynamicSortFilter(true);
    m_historyProxy->setFilterKeyColumn(HistoryTableModel::COLUMN_LINEID);
    m_historyProxy->sort(HistoryTableModel::COLUMN_CALLSTART,
                         Qt::DescendingOrder);

    connectAll();
}

void DialerApplication::modemConnected()
{
    TRACE
    //TODO: Handle multiple modems
    if (m_manager->modem() && m_manager->modem()->isValid())
        m_modem = m_manager->modem();
}

void DialerApplication::modemDisconnected()
{
    TRACE
    //TODO: Handle multiple modems
}

void DialerApplication::networkConnected()
{
    TRACE
    if (m_manager->network() && m_manager->network()->isValid())
        m_network = m_manager->network();
}

void DialerApplication::networkDisconnected()
{
    TRACE
}

void DialerApplication::callManagerConnected()
{
    TRACE
    if (m_manager->callManager() && m_manager->callManager()->isValid())
        m_callManager = m_manager->callManager();


    qDebug() << QString("Disconnect calls changed signal");
    disconnect(m_callManager, SIGNAL(callsChanged()));

    qDebug() << QString("Disconnect incoming signal");
    disconnect(m_callManager, SIGNAL(incomingCall(CallItem*)));

    qDebug() << QString("Disconnect resource lost");
    disconnect(m_callManager, SIGNAL(callResourceLost(const QString)));
}

void DialerApplication::callManagerDisconnected()
{
    TRACE
    qDebug() << QString("CallMgr disconnected");
}

void DialerApplication::messagesWaitingChanged()
{
    TRACE
    static MNotification *vmail = NULL;

    if (!m_manager->voicemail() || !m_manager->voicemail()->isValid()) {
        qDebug() << QString("Voicemail proxy is invalid, ignoring");
        return;
    }


    if (!vmail) {
        bool found = false;
        foreach (MNotification *notice, MNotification::notifications()) {
            if (notice->eventType() == MNotification::MessageArrivedEvent) {
                // If we've already found a MessageArrived notification,
                // we must delete others since we only want one
                if (found) {
                    qDebug() << QString("Removing duplicate voicemail notice");
                    notice->remove();
                    delete notice;
                }
                else {
                    vmail = notice;
                    found = true;
                }
            }
            else {
                // We're only interested in MessageArrived events, all others
                // can need to be deleted here since they are copies
                delete notice;
            }
        }

        if (!found) {
            if (!m_manager->voicemail()->waiting()) {
                qDebug() << QString("No waiting Voicemail messages");
                return;
            }
            else {
                // This is the first instance of a MessageArrived event
                qDebug() << QString("Creating new voicemail notice instance");
                vmail = new MNotification(MNotification::MessageArrivedEvent);
                vmail->setCount(0);
            }
        }
        else {
            if (!m_manager->voicemail()->waiting()) {
                qDebug() << QString("No waiting Voicemail messages");
                vmail->remove();
                return;
            }
        }
    }

    // We've got a valid notification and we have messages waiting...
    int vCount = m_manager->voicemail()->count();
    int nCount = vmail->count();

    if (vCount < 0) vCount = 0;
    if (nCount < 0) nCount = 0;

    qDebug() << QString("Voicemails: %1").arg(QString::number(vCount));
    qDebug() << QString("Notices:    %1").arg(QString::number(nCount));

    // No more Voicemail messages waiting, remove notification
    if (vCount <= 0) {
        qDebug() << QString("No Voicemails waiting, removing notification");
        vmail->remove();
        return;
    }

    // The waiting voicemail count has changed, update and [re]publish it
    //% "You have %1 voice messages"
    vmail->setSummary(qtTrId("xx_messages_waiting").arg(vCount));
    vmail->setImage("icon-m-telephony-voicemail");
    vmail->setCount(vCount);
    qDebug() << QString("Voicemail count changed, publishing notification");
    vmail->publish();
}

void DialerApplication::onCallsChanged()
{
    TRACE
    this->setPrestarted(false);
}
