/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MANAGERPROXY_H
#define MANAGERPROXY_H

#include "manager_interface.h"
#include "modemproxy.h"
#include "networkproxy.h"
#include "callmanager.h"
#include "resourceproxy.h"
#include <QtDBus>
#include <QDebug>

#define OFONO_SERVICE "org.ofono"
#define OFONO_MANAGER_PATH "/"
#define OFONO_HISTORY_PATH "/"

class HistoryProxy: public org::ofono::CallHistory
{
    Q_OBJECT
    Q_PROPERTY(QSettings* cache READ cache)

public:
    virtual ~HistoryProxy();
    static HistoryProxy *instance();

public slots:
    QSettings* cache() const;

Q_SIGNALS:
    void historyChanged(QStringList ids);

private slots:
    void sendMissedCallNotification(QList<CallHistoryEvent> missed);
    void getHistoryFinished(QDBusPendingCallWatcher *call);
    void voiceHistoryChanged(uint count);
    void initCache();

protected:
    HistoryProxy(const QString &service=OFONO_SERVICE,
                 const QString &path=OFONO_HISTORY_PATH,
                 const QDBusConnection &connection=QDBusConnection::systemBus(),
                 QObject *parent = 0);

private:
    HistoryProxy(const HistoryProxy&);
    HistoryProxy& operator= (const HistoryProxy&);

    QSettings         *m_cache;

    static HistoryProxy *gHistory;
};

class ManagerProxy: public org::ofono::Manager
{
    Q_OBJECT
    Q_PROPERTY(ModemProxy* modem READ modem)
    Q_PROPERTY(NetworkProxy* network READ network)

public:
    virtual ~ManagerProxy();

    static ManagerProxy *instance();

    ModemProxy* modem() const;
    NetworkProxy* network() const;
    CallManager* callManager() const;
    VolumeManager* volumeManager() const;
    VoicemailProxy* voicemail() const;

public slots:
    void managerDBusGetModemsDone(QDBusPendingCallWatcher *call);

private Q_SLOTS:
    void modemAdded(const QDBusObjectPath &in0, const QVariantMap &in1);
    void modemRemoved(const QDBusObjectPath &in0);

protected:
    ManagerProxy(const QString &service=OFONO_SERVICE,
                 const QString &path=OFONO_MANAGER_PATH,
                 const QDBusConnection &connection=QDBusConnection::systemBus(),
                 QObject *parent = 0);

private:
    ManagerProxy(const ManagerProxy&);
    ManagerProxy& operator= (ManagerProxy&);

    QString       m_modemPath;
    ModemProxy   *m_modem;
    NetworkProxy *m_network;
    CallManager  *m_callManager;
    VolumeManager *m_volumeManager;
    VoicemailProxy *m_voicemail;

    static ManagerProxy *gManager;
};

#endif
