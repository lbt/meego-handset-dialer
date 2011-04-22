/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef DIALERAPPLICATION_h
#define DIALERAPPLICATION_h

#include "managerproxy.h"
#include "mainwindow.h"
#include "historytablemodel.h"
#include <seaside.h>
#include <seasidesyncmodel.h>
#include <seasideproxymodel.h>
#include <MApplication>
#include <MApplicationService>
#include <MGConfItem>
#include <MButtonGroup>
#include <QSortFilterProxyModel>

#define DA_SEASIDEMODEL DialerApplication::instance()->seasideModel()
#define DA_SEASIDEPROXY DialerApplication::instance()->seasideProxy()
#define DA_HISTORYMODEL DialerApplication::instance()->historyModel()
#define DA_HISTORYPROXY DialerApplication::instance()->historyProxy()

class DialerApplication: public MApplication
{
    Q_OBJECT

public:

    DialerApplication(int &argc, char **argv);
    DialerApplication(int &argc, char **argv, MApplicationService *service);
    bool isConnected();
    void setError(const QString msg);
    QString lastError();
    int showErrorDialog();
    int showErrorDialog(const QString msg);
    static DialerApplication     *instance();
    SeasideSyncModel      *seasideModel();
    SeasideProxyModel     *seasideProxy();
    HistoryTableModel     *historyModel();
    QSortFilterProxyModel *historyProxy();
    MButtonGroup          *headerButtonGroup();

    virtual void releasePrestart();
    virtual void restorePrestart();

private Q_SLOTS:
    void modemConnected();
    void networkConnected();
    void callManagerConnected();
    void modemDisconnected();
    void networkDisconnected();
    void callManagerDisconnected();
    void messagesWaitingChanged();
    void createMainWindow();
    QStringList dumpDisplayInfo();
    void switchPage(int id);
    void switchPageNow(int id);
    void connectAll();
    void handleCallsChanged();

private:
    void init();

    ManagerProxy *m_manager;
    ModemProxy   *m_modem;
    NetworkProxy *m_network;
    CallManager  *m_callManager;
    MainWindow   *m_mainWindow;
    bool          m_connected;
    QString       m_lastError;

    SeasideSyncModel      *m_seasideModel;
    SeasideProxyModel     *m_seasideProxy;
    HistoryTableModel     *m_historyModel;
    QSortFilterProxyModel *m_historyProxy;

    MButtonGroup        *m_header;
    MGConfItem          *m_lastPage;

    Q_DISABLE_COPY(DialerApplication);
};

#endif // DIALERAPPLICATION_H
