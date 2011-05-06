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
    m_notification(new NotificationDialog()),
    m_keypad(0),
    m_acceptAction(DBUS_SERVICE, DBUS_SERVICE_PATH, DBUS_SERVICE, "accept"),
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
        setPortraitOrientation();
    setOrientationLocked(true);
    setToolbarViewType(MToolBar::tabType);
    m_pages.clear();

    connect (this, SIGNAL(displayEntered()), this, SLOT(onDisplayEntered()));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    TRACE
    if (closeOnLazyShutdown()) {
        setCloseOnLazyShutdown(false);
        qDebug("Lazy shutdown - hide the window");
    }
    event->accept();
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

void MainWindow::onDisplayEntered()
{
    TRACE
    if (m_incomingCall) {
        m_alert->setCallItem(m_incomingCall);
        m_alert->appear();
    }
    CallManager *cm = ManagerProxy::instance()->callManager();
    if (cm && cm->isValid()) {
        qDebug() << QString("CALLING UPDATEBUTTONS onEnterDisplayEvent!!!!");
        keypad()->updateButtons();
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
        if (call && call->isValid() && !call->lineID().isEmpty()) {
            // Save this for when RemoteAction "accept" is called or onDisplay
            // changes, but make sure we null it out if the state changes before
            //user takes action
            m_incomingCall = call;
            connect(m_incomingCall,SIGNAL(stateChanged()),SLOT(callStateChanged()));
        }
        activateWindow();
    }
}

void MainWindow::handleResourceUnavailability(const QString message)
{
    TRACE

    DialerApplication *app = DialerApplication::instance();

    if (isOnDisplay()) {
	m_notification->setNotification(message);
	m_notification->appear();
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

    activateWindow();
}

void MainWindow::showTBD()
{
    TRACE

    if (!m_tbd)
        //% "This feature is not yet implemented"
        m_tbd = new MMessageBox(qtTrId("xx_not_yet_implemented"));
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
