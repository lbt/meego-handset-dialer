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
#include "dialerpage.h"
#include "recentpage.h"
#include "peoplepage.h"
#include "favoritespage.h"
#include "debugpage.h"
#include <QApplication>
#include <QDesktopWidget>
#include <MDialog>
#include <MImageWidget>
#include <MButton>
#include <MLabel>
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MStylableWidget>
#include <MNotification>
#include <MToolBar>
#include <MSceneManager>
#include <MWidgetAction>
#include <QDateTime>
#include "dialer_adaptor.h"

MainWindow::MainWindow() :
    MApplicationWindow(),
    m_configLastPage(new MGConfItem("/apps/dialer/lastPage", this)),
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

MainWindow* MainWindow::instance()
{
    TRACE
    static MainWindow *_instance = NULL;

    if(_instance == NULL)
    {
        _instance = new MainWindow;
        _instance->setupUi();
    }

    return _instance;
}

void MainWindow::setupUi()
{
    TRACE
    qDebug() << dumpDisplayInfo().join("\n");

    if (this->orientation() != M::Portrait)
        this->setOrientationAngle(M::Angle270);

    // TODO: If we *REALLY* only support portrait, need to uncomment next line
    //m_mainWindow->setKeepCurrentOrientation(true);

    // Create the buttons and button group to be put in the toolbar
    MButton *dial, *recent, *people, *favs;
    MWidgetAction *dAction, *rAction, *pAction, *fAction;

    // Dial button
    dAction = new MWidgetAction(this);
    dAction->setLocation(MAction::ToolBarLocation);
    dAction->setCheckable(true);
    dial = new MButton();
    dial->setObjectName("headerButton");
    dial->setViewType("toolbartab");
    dial->setCheckable(true);
    dial->setIconID("icon-dialer-phone-off");
    dial->setToggledIconID("icon-dialer-phone-on");
    dAction->setWidget(dial);

    // Recent button
    rAction = new MWidgetAction(this);
    rAction->setCheckable(true);
    rAction->setLocation(MAction::ToolBarLocation);
    recent = new MButton();
    recent->setObjectName("headerButton");
    recent->setViewType("toolbartab");
    recent->setCheckable(true);
    recent->setIconID("icon-dialer-history-off");
    recent->setToggledIconID("icon-dialer-history-on");
    rAction->setWidget(recent);

    // People button
    pAction = new MWidgetAction(this);
    pAction->setCheckable(true);
    pAction->setLocation(MAction::ToolBarLocation);
    people = new MButton();
    people->setObjectName("headerButton");
    people->setViewType("toolbartab");
    people->setCheckable(true);
    people->setIconID("icon-dialer-people-off");
    people->setToggledIconID("icon-dialer-people-on");
    pAction->setWidget(people);

    // Favorites button
    fAction = new MWidgetAction(this);
    fAction->setCheckable(true);
    fAction->setLocation(MAction::ToolBarLocation);
    favs = new MButton();
    favs->setObjectName("headerButton");
    favs->setViewType("toolbartab");
    favs->setCheckable(true);
    favs->setIconID("icon-dialer-favorite-off");
    favs->setToggledIconID("icon-dialer-favorite-on");
    fAction->setWidget(favs);

    // Button group
    m_header = new MButtonGroup();
    m_header->addButton(dial,   (int)GenericPage::PAGE_DIALER);
    m_header->addButton(recent, (int)GenericPage::PAGE_RECENT);
    m_header->addButton(people, (int)GenericPage::PAGE_PEOPLE);
    m_header->addButton(favs,   (int)GenericPage::PAGE_FAVORITE);

    // Create pages
    m_pages << dynamic_cast<GenericPage *>(new DialerPage())
            << dynamic_cast<GenericPage *>(new RecentPage())
            << dynamic_cast<GenericPage *>(new PeoplePage())
            << dynamic_cast<GenericPage *>(new FavoritesPage())
            << dynamic_cast<GenericPage *>(new DebugPage());

    qDebug() << QString("After creating pages...count - %1").arg(QString::number(m_pages.length()));

    foreach(GenericPage *page, m_pages)
    {
        page->addAction(dAction);
        page->addAction(rAction);
        page->addAction(pAction);
        page->addAction(fAction);
    }

    QObject::connect(m_header, SIGNAL(buttonClicked(int)), this, SLOT(switchPage(int)));

    // Jump to the last shown page on startup, default to DialerPage otherwise
    int lastPage = m_configLastPage->value(QVariant(GenericPage::PAGE_DIALER)).toInt();
    switchPageNow(lastPage);
    m_header->button(lastPage)->click(); // Set ButtonGroup state to match page

    QObject::connect(DialerApplication::instance(), SIGNAL(prestartReleased()), SLOT(showUi()));
    QObject::connect(DialerApplication::instance(), SIGNAL(prestartRestored()), SLOT(hideUi()));

    CallManager *cm = ManagerProxy::instance()->callManager();

    qDebug() << QString("Connect calls changed");
    connect(cm, SIGNAL(callsChanged()), SLOT(handleCallsChanged()));

    qDebug() << QString("Connect incoming call");
    connect(cm, SIGNAL(incomingCall(CallItem*)), SLOT(handleIncomingCall(CallItem*)));

    qDebug() << QString("Connect resource unavailable");
    connect(cm, SIGNAL(callResourceLost(const QString)), SLOT(handleResourceUnavailability(const QString)));

    this->show();
}

void MainWindow::showUi()
{
    foreach(GenericPage *page, m_pages)
    {
        page->activateWidgets();
    }
}

void MainWindow::hideUi()
{
    foreach(GenericPage *page, m_pages)
    {
        page->deactivateAndResetWidgets();
    }
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

void  MainWindow::handleCallsChanged()
{
    TRACE
    ManagerProxy *mp = ManagerProxy::instance();
    DialerApplication *ap = DialerApplication::instance();

    if(ap->isPrestarted())
        ap->setPrestarted(false);

    this->activateWindow();

    if(mp->callManager() && mp->callManager()->isValid() && this->keypad()) {
        this->keypad()->updateButtons();
    }
}

int MainWindow::showErrorDialog(const QString msg)
{
    //FIXME: Bit of a nasty hack.
    DialerApplication::instance()->setError(msg);
    return showErrorDialog();
}

int MainWindow::showErrorDialog()
{
    TRACE
    static MLabel *msg = 0;
    static MDialog *dialog = 0;

    if (!this->isVisible()) this->show();

    if (!msg) {
        msg = new MLabel();
        msg->setObjectName("errorMsg");
        msg->setWordWrap(true);
    }
    if (!dialog) {
        //% "Error"
        dialog = new MDialog(qtTrId("xx_error"),M::IgnoreButton|M::AbortButton);
        dialog->setObjectName("errorDialog");
        dialog->setCloseButtonVisible(false);
        dialog->setCentralWidget(msg);
    }

    msg->setText(DialerApplication::instance()->lastError());

    dialog->appear();

    return dialog->exec(this);
}

void MainWindow::switchPage(int id)
{
    TRACE
    qDebug() << QString("Switching to page %1").arg(id);
    this->m_pages.at(id)->appear();
    m_configLastPage->set(QVariant(id));
}

void MainWindow::switchPageNow(int id)
{
    TRACE
    qDebug() << QString("Switching to page %1").arg(id);
    this->m_pages.at(id)->appear();
    m_configLastPage->set(QVariant(id));
}

QStringList MainWindow::dumpDisplayInfo()
{
    TRACE
    static QStringList m_displayInfo;
    QDesktopWidget *desktop = QApplication::desktop();

    if (m_displayInfo.isEmpty()) {
        QSize s = this->sceneManager()->visibleSceneSize();
        double dppmm_w = 1.0 * desktop->width() / desktop->widthMM();
        double dppmm_h = 1.0 * desktop->height() / desktop->heightMM();
        m_displayInfo << QString("Scene   %1 x %2 mm (%3 x %4 px)")
                         .arg(int(s.width()/dppmm_w))
                         .arg(int(s.height()/dppmm_h))
                         .arg(s.width())
                         .arg(s.height());
        m_displayInfo << QString("Desktop %1 x %2 mm (%3 x %4 px)")
                         .arg(desktop->widthMM())
                         .arg(desktop->heightMM())
                         .arg(desktop->width())
                         .arg(desktop->height());
        m_displayInfo << QString("Display %1 x %2 px/mm")
                         .arg(dppmm_w, 0, 'f', 2)
                         .arg(dppmm_h, 0, 'f', 2);
    }
    return m_displayInfo;
}

MButtonGroup* MainWindow::headerButtonGroup()
{
    TRACE
    return m_header;
}
