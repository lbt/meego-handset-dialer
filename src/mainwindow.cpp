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
#include "mainwindow.h"
#include "dialerapplication.h"
#include "managerproxy.h"
#include "genericpage.h"
#include <MDialog>
#include <MImageWidget>
#include <MButton>
#include <MLabel>
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MStylableWidget>
#include <MNotification>
#include <MToolBar>
#include <QDateTime>
#include "dialer_adaptor.h"

MainWindow::MainWindow() :
    MApplicationWindow(),
    m_lastPage(0),
    m_alert(new AlertDialog()),
    m_search(new SearchBar()),
    m_keypad(0),
    m_incomingCall(0),
    m_tbd(0)
{
    new DialerAdaptor(this);

    QDBusConnection connection = QDBusConnection::sessionBus();

    if(!connection.registerObject(DBUS_SERVICE_PATH,this)){
	qCritical()<<"Error registering dbus object: "<<
				connection.lastError().message();
    }

    TRACE
    if (orientation() != M::Portrait)
        setOrientationAngle(M::Angle270);
    // TODO: If we *REALLY* only support portrait, need to uncomment next line
    setOrientationLocked(true);
    setToolbarViewType(MToolBar::tabType);
    m_pages.clear();
    m_search->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                        QSizePolicy::MinimumExpanding));

    MRemoteAction m_acceptAction = MRemoteAction(DBUS_SERVICE,
                                                 DBUS_SERVICE_PATH,
                                                 DBUS_SERVICE,
                                                 "accept");
}

void MainWindow::showDebugPage()
{
    TRACE
    if (currentPage() == m_pages.at(GenericPage::PAGE_DEBUG)) {
        if (m_lastPage)
            m_lastPage->appear();
        else
            m_pages.at(GenericPage::PAGE_DEBUG)->disappear();
    } else {
        m_lastPage = currentPage();
        m_pages.at(GenericPage::PAGE_DEBUG)->appear();
    }
}

void MainWindow::handleIncomingCall(CallItem *call)
{
    TRACE
    qDebug("Handling an incoming call...");
    if (isOnDisplay()) {
        m_alert->setCallItem(call);
        m_alert->appear();
    } else {
        QString name;
        QString photo  = DEFAULT_AVATAR_ICON;
        QString lineid = "Private";
        QString summary("Incoming call");
        QString body;
        MNotification notice(NOTIFICATION_CALL_EVENT);

        if (call && call->isValid() && !call->lineID().isEmpty()) {
            lineid = stripLineID(call->lineID());
            SeasideSyncModel *contacts = DA_SEASIDEMODEL;
            QModelIndex first = contacts->index(0,Seaside::ColumnPhoneNumbers);
            QModelIndexList matches = contacts->match(first, Qt::DisplayRole,
                                                      QVariant(lineid),1);
            if (!matches.isEmpty()) {
                QModelIndex person = matches.at(0); //First match wins
                SEASIDE_SHORTCUTS
                SEASIDE_SET_MODEL_AND_ROW(person.model(), person.row());
                name = SEASIDE_FIELD(Name, String);
                photo = SEASIDE_FIELD(Avatar, String);
            }
            // Save this for when RemoteAction "accept" is called, but make sure
            // we null it out if the state changes before user takes action
            m_incomingCall = call;
            connect(m_incomingCall,SIGNAL(stateChanged()),SLOT(callStateChanged()));
        } else {
            lineid = "Unavailable";
        }

        body = QString("You have an incoming call from %1")
                              .arg(name.isEmpty()?lineid:name);

        notice.setSummary(summary);
        notice.setBody(body);
        notice.setImage(photo);
        notice.setAction(m_acceptAction);
        notice.publish();

        qDebug() << QString("%1: %2").arg(summary).arg(body);
    }
}

void MainWindow::callStateChanged()
{
    TRACE
    if (m_incomingCall && m_incomingCall->isValid()) {
        disconnect(m_incomingCall, SIGNAL(stateChanged()));
        m_incomingCall = NULL;
    }
}

void MainWindow::call(QString no)
{
    TRACE
    m_lastPage = currentPage();
    if(!m_pages.size())
    {
	qDebug("DialerPage probably hasn't been created yet.");
	return;
    }

    m_pages.at(GenericPage::PAGE_DIALER)->appear();

    ManagerProxy *mp = ManagerProxy::instance();

    if(!mp->callManager()->isValid()) return;

    mp->callManager()->dial(no);

    showMaximized();
}

void MainWindow::accept()
{
    TRACE
    if (!m_incomingCall || !m_incomingCall->isValid())
        return;

    // A call is "waiting" if there is an active call already and a new
    // call comes in.  This is handled by the call manager object instead
    if (m_incomingCall->state() == CallItemModel::STATE_WAITING) {
        CallManager *cm = ManagerProxy::instance()->callManager();
        if (cm && cm->isValid())
            cm->holdAndAnswer();
        else
            qCritical("CallManager is invalid, cannot answer waiting calls");
    } else {
        m_incomingCall->callProxy()->answer();
    }

    // Switch to the dialer page where call views are shown
    if(m_pages.size())
        m_pages.at(GenericPage::PAGE_DIALER)->appear();

    showMaximized();
}

void MainWindow::showTBD()
{
    TRACE

    if (!m_tbd)
        m_tbd = new MMessageBox("This feature is not yet implemented");
    m_tbd->exec(this);
}

void MainWindow::simulateIncomingCall()
{
    TRACE
    ManagerProxy *mp = ManagerProxy::instance();
    qDebug("Dialing \"199\" to trigger phonesim to simulate an incoming call");
    if (mp->isValid() && mp->callManager() && mp->callManager()->isValid())
        mp->callManager()->dial("199"); // Invoke phonesim dialBack()
}

bool MainWindow::event(QEvent *event)
{
    TRACE
    if (QEvent::KeyPress == event->type()) {
        QKeyEvent *kev = (QKeyEvent *)event;

        // Trap "F1" as trigger to show debug/info page
        if (Qt::Key_F1 == kev->key()) {
            MainWindow::showDebugPage();
            return true;
        // Trap "F10" as trigger to simulate an incoming call event
        } else if (Qt::Key_F10 == kev->key()) {
            MainWindow::simulateIncomingCall();
            return true;
        }
    }
    return MApplicationWindow::event(event);
}

SearchBar *MainWindow::searchBar()
{
    TRACE
    return m_search;
}

DialerKeyPad *MainWindow::keypad()
{
    TRACE
    if (!m_keypad) {
	m_keypad = new DialerKeyPad();
        m_keypad->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                            QSizePolicy::MinimumExpanding));
    }
    return m_keypad;
}
