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
    init();
}

bool DialerApplication::isConnected()
{
    TRACE
    return m_connected;
}

QString DialerApplication::lastError()
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
    m_mainWindow = 0;
    m_header = 0;
    m_connected = false;
    m_lastError = QString();

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

    m_manager = ManagerProxy::instance();
    if (!m_manager || !m_manager->isValid())
        //% "Failed to connect to org.ofono.Manager: is ofonod running?"
        setError(qtTrId("xx_no_ofono_error"));
    else
        m_connected = true;

    m_lastPage = new MGConfItem("/apps/dialer/lastPage");

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
    connect(m_manager->voicemail(), SIGNAL(messagesWaitingChanged()),
                                      SLOT(messagesWaitingChanged()));
}

void DialerApplication::modemConnected()
{
    TRACE
    if (m_manager->modem() && m_manager->modem()->isValid())
        m_modem = m_manager->modem();
}

void DialerApplication::modemDisconnected()
{
    TRACE
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

    // We now have enough to get started with GUI stuff
    createMainWindow();
}

void DialerApplication::callManagerDisconnected()
{
    TRACE
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

int DialerApplication::showErrorDialog()
{
    TRACE
    static MLabel *msg = 0;
    static MDialog *dialog = 0;

    if (!m_mainWindow)
        m_mainWindow = new MainWindow();

    if (!m_mainWindow->isVisible())
        m_mainWindow->show();

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

    msg->setText(lastError());

    dialog->appear();

    return dialog->exec(m_mainWindow);
}

void DialerApplication::createMainWindow()
{
    TRACE
    disconnect(m_callManager, SIGNAL(incomingCall(CallItem*)));

    if (!m_mainWindow)
        m_mainWindow = new MainWindow();

    connect(m_callManager, SIGNAL(incomingCall(CallItem*)),
            m_mainWindow,  SLOT(handleIncomingCall(CallItem*)));

    m_mainWindow->show();

    qDebug() << dumpDisplayInfo().join("\n");

    if (m_mainWindow->orientation() != M::Portrait)
        m_mainWindow->setOrientationAngle(M::Angle270);
    // TODO: If we *REALLY* only support portrait, need to uncomment next line
    //m_mainWindow->setKeepCurrentOrientation(true);

    // Create the buttons and button group to be put in the toolbar
    MButton *dial, *recent, *people, *favs;
    MWidgetAction *dAction, *rAction, *pAction, *fAction;
    {
        // Dial button
        dAction = new MWidgetAction(m_mainWindow);
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
        rAction = new MWidgetAction(m_mainWindow);
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
        pAction = new MWidgetAction(m_mainWindow);
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
        fAction = new MWidgetAction(m_mainWindow);
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
    }

    // Create pages
    m_mainWindow->m_pages << dynamic_cast<GenericPage *>(new DialerPage())
                          << dynamic_cast<GenericPage *>(new RecentPage())
                          << dynamic_cast<GenericPage *>(new PeoplePage())
                          << dynamic_cast<GenericPage *>(new FavoritesPage())
                          << dynamic_cast<GenericPage *>(new DebugPage());

    for (int i=0; i < m_mainWindow->m_pages.length(); i++) {
        if (!m_mainWindow->m_pages.isEmpty() && m_mainWindow->m_pages.at(i)) {
            m_mainWindow->m_pages.at(i)->addAction(dAction);
            m_mainWindow->m_pages.at(i)->addAction(rAction);
            m_mainWindow->m_pages.at(i)->addAction(pAction);
            m_mainWindow->m_pages.at(i)->addAction(fAction);
        }
    }

    QObject::connect(m_header,  SIGNAL(buttonClicked(int)),
                     this,    SLOT(switchPage(int)));

    // Jump to the last shown page on startup, default to DialerPage otherwise
    int lastPage =m_lastPage->value(QVariant(GenericPage::PAGE_DIALER)).toInt();
    switchPageNow(lastPage);
    m_header->button(lastPage)->click(); // Set ButtonGroup state to match page
}

void DialerApplication::switchPage(int id)
{
    TRACE
    qDebug() << QString("Switching to page %1").arg(id);
    m_mainWindow->m_pages.at(id)->appear();
    m_lastPage->set(QVariant(id));
}

void DialerApplication::switchPageNow(int id)
{
    TRACE
    qDebug() << QString("Switching to page %1").arg(id);
    m_mainWindow->m_pages.at(id)->appear();
    m_lastPage->set(QVariant(id));
}

QStringList DialerApplication::dumpDisplayInfo()
{
    TRACE
    static QStringList m_displayInfo;

    if (m_displayInfo.isEmpty()) {
        QSize s = m_mainWindow->sceneManager()->visibleSceneSize();
        double dppmm_w = 1.0 * desktop()->width() / desktop()->widthMM();
        double dppmm_h = 1.0 * desktop()->height() / desktop()->heightMM();
        m_displayInfo << QString("Scene   %1 x %2 mm (%3 x %4 px)")
                         .arg(int(s.width()/dppmm_w))
                         .arg(int(s.height()/dppmm_h))
                         .arg(s.width())
                         .arg(s.height());
        m_displayInfo << QString("Desktop %1 x %2 mm (%3 x %4 px)")
                         .arg(desktop()->widthMM())
                         .arg(desktop()->heightMM())
                         .arg(desktop()->width())
                         .arg(desktop()->height());
        m_displayInfo << QString("Display %1 x %2 px/mm")
                         .arg(dppmm_w, 0, 'f', 2)
                         .arg(dppmm_h, 0, 'f', 2);
    }
    return m_displayInfo;
}

MButtonGroup *DialerApplication::headerButtonGroup()
{
    TRACE
    return m_header;
}
