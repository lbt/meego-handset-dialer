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
#ifdef IVI_HFP
#include "dialerpage.h"
#endif
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
#include <MInfoBanner>
#ifdef IVI_HFP
#include <QTimer>
#endif
#include "dialer_adaptor.h"

MainWindow::MainWindow() :
    MApplicationWindow(),
    m_lastPage(0),
    m_alert(new AlertDialog()),
#ifdef IVI_HFP
    m_bluetoothDialog(new BluetoothDialog()),
    m_timer(0),
#endif
    m_search(new SearchBar()),
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
#ifdef IVI_HFP
    if (orientation() != M::Landscape)
#else
    if (orientation() != M::Portrait)
#endif
        setOrientationAngle(M::Angle270);
    // TODO: If we *REALLY* only support portrait, need to uncomment next line
    setOrientationLocked(true);
    setToolbarViewType(MToolBar::tabType);
    m_pages.clear();
    m_search->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                        QSizePolicy::MinimumExpanding));

#ifdef IVI_HFP
    connect(m_bluetoothDialog, SIGNAL(modemChanged(QString)),
            this, SLOT(modemChanged(QString)));
#endif
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
                name = QString("%1, %2").arg(SEASIDE_FIELD(LastName, String))
                                        .arg(SEASIDE_FIELD(FirstName, String));
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
#ifdef IVI_HFP
        // Trap "F2" as trigger to select modem
        } else if (Qt::Key_F2 == kev->key()) {
            MainWindow::showBluetoothDialog();
            return true;
#endif
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

#ifdef IVI_HFP
void MainWindow::showBluetoothDialog()
{
    TRACE
    qDebug("Handling an bluetooth selection...");
    ManagerProxy *mp = ManagerProxy::instance();

    if (mp->isValid())
    {
        m_bluetoothDialog->appear();
    }
}

void MainWindow::modemChanged(QString path)
{
    TRACE
    ManagerProxy *mp = ManagerProxy::instance();
    if (mp->isValid())
    {
        /* create a new ManagerProxy instance */
        qDebug() << QString("Changing modem:");

        delete mp;
        ManagerProxy *mp = ManagerProxy::instance();
        mp->setModem(path);

        if (mp->modem() && mp->modem()->isValid())
        {
            qDebug() << QString("Connec to new modem: ") << mp->modem()->path();
            connect(mp->modem(), SIGNAL(connected()), this,
                                        SLOT(modemConnected()));
            connect(mp->modem(), SIGNAL(disconnected()), this,
                                        SLOT(modemDisconnected()));
        }

        MGConfItem *preferedModem = new MGConfItem("/apps/hfdialer/preferedModem");
        preferedModem->set(QVariant(path));
    }
}

void MainWindow::modemConnected()
{
    TRACE
    qDebug() << QString("Modem connected: ");
    ManagerProxy *mp = ManagerProxy::instance();
    if (mp->isValid() && mp->modem() && mp->modem()->isValid())
    {

        if (mp->modem()->powered())
        {
            /* connect now, modem is enabled */
            qDebug() << QString("Modem is powered: ");
            this->connectCallManager();
        }
        else
        {
            /* enable the modem first, defer connect */
            qDebug() << QString("Modem is not powered: set property to true");
            connect(mp->modem(), SIGNAL(poweredChanged(bool)), this,
                                        SLOT(modemIsPowered(bool)));
            mp->modem()->setPowered(true);
        }
    }
}

void MainWindow::modemDisconnected()
{
    TRACE
    qDebug() << QString("Modem disconnected");
}

void MainWindow::modemIsPowered(bool isPowered)
{
    qDebug() << QString("Modem Powered: ") << isPowered;
    if(isPowered)
    {
        this->connectCallManager();
    }
}

void MainWindow::networkConnected()
{
    TRACE
    qDebug() << QString("Network connected");
}

void MainWindow::networkDisconnected()
{
    TRACE
    qDebug() << QString("Network disconnected");
}

void MainWindow::callManagerConnected()
{
    TRACE
    qDebug() << QString("CallManager connected");
    ManagerProxy *mp = ManagerProxy::instance();
    if (mp->callManager() && mp->callManager()->isValid()) {
        connect(mp->callManager(), SIGNAL(incomingCall(CallItem*)),
                this,  SLOT(handleIncomingCall(CallItem*)));

        connect (mp->callManager(), SIGNAL(callsChanged()),
                 keypad(), SLOT(callsChanged()));
        this->displayBannerMessage("Handsfree Connected");
        if(m_timer)
        {
            if (m_timer->isActive())
                m_timer->stop();
            delete m_timer;
            m_timer = NULL;
        }
    }
}

void MainWindow::callManagerDisconnected()
{
    TRACE
    qDebug() << QString("CallManager disconnected");
    this->displayBannerMessage("Handsfree Disconnected");
}

void MainWindow::connectCallManager()
{
    qDebug() << QString("Connect to CallManager");
    ManagerProxy *mp = ManagerProxy::instance();
    if (mp && mp->isValid() && !mp->modem()->path().isEmpty())
    {
        if (m_timer)
        {
            if (m_timer->isActive())
                m_timer->stop();
            delete m_timer;
            m_timer = NULL;
        }
        m_tries = 2;    // try connecting 2 times
        m_timer = new QTimer(this);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(connectCallManagerTimerDone()));
        m_timer->start(2000); // wait for 2 seconds for retry
    }
}

void MainWindow::connectCallManagerTimerDone()
{
    qDebug() << QString("Trying to connect to CallManager....") << m_tries;
    ManagerProxy *mp = ManagerProxy::instance();
    if (m_tries >0 && mp && mp->isValid() && !mp->modem()->path().isEmpty())
    {
        mp->setNetwork(mp->modem()->path());
        mp->setCallManager(mp->modem()->path());

        disconnect(mp->callManager(), SIGNAL(incomingCall(CallItem*)));

        connect(mp->network(), SIGNAL(connected()), this,
                                      SLOT(networkConnected()));
        connect(mp->network(), SIGNAL(disconnected()), this,
                                      SLOT(networkDisconnected()));
        connect(mp->callManager(), SIGNAL(connected()), this,
                                          SLOT(callManagerConnected()));
        connect(mp->callManager(), SIGNAL(disconnected()), this,
                                          SLOT(callManagerDisconnected()));

        DialerPage* page = dynamic_cast<DialerPage*>(m_pages.at(GenericPage::PAGE_DIALER));
        connect(mp->callManager(), SIGNAL(connected()), page,
                                            SLOT(updateCallManager()));
        m_tries--;
    }
    else
    {
        if (m_timer->isActive())
            m_timer->stop();
        delete m_timer;
        m_timer = NULL;
    }
}
#endif

#ifdef IVI_HFP
void MainWindow::displayBannerMessage(QString msg)
{
    MInfoBanner* infoBanner = new MInfoBanner(MInfoBanner::Information);
    infoBanner->setObjectName("popupMessage");
    infoBanner->setBodyText(msg);
    infoBanner->appear(MSceneWindow::DestroyWhenDone);

    QTimer::singleShot(1000, infoBanner, SLOT(disappear()));
}
#endif
