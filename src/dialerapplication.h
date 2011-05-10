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

    Q_PROPERTY(bool isConnected READ isConnected)
    Q_PROPERTY(bool hasError READ hasError)
    Q_PROPERTY(QString lastError READ lastError)

public:
    DialerApplication(int &argc, char **argv);
    DialerApplication(int &argc, char **argv, MApplicationService *service);

    static DialerApplication* instance();

    virtual void releasePrestart();
    virtual void restorePrestart();

    bool hasError() const;
    QString lastError() const;
    void setError(const QString msg);

    bool isConnected();

    SeasideSyncModel      *seasideModel();
    SeasideProxyModel     *seasideProxy();
    HistoryTableModel     *historyModel();
    QSortFilterProxyModel *historyProxy();

Q_SIGNALS:
    void showUi();
    void hideUi();

private Q_SLOTS:
    void connectAll();
    void messagesWaitingChanged();

    void modemConnected();
    void modemDisconnected();
    void networkConnected();
    void networkDisconnected();
    void callManagerConnected();
    void callManagerDisconnected();

    void onCallsChanged();

private:
    void init();

    ManagerProxy *m_manager;
    ModemProxy   *m_modem;
    NetworkProxy *m_network;
    CallManager  *m_callManager;

    bool          m_connected;
    QString       m_lastError;

    SeasideSyncModel      *m_seasideModel;
    SeasideProxyModel     *m_seasideProxy;

    HistoryTableModel     *m_historyModel;
    QSortFilterProxyModel *m_historyProxy;

    Q_DISABLE_COPY(DialerApplication);
};

#endif // DIALERAPPLICATION_H
