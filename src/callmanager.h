/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef CALLMANAGER_H
#define CALLMANAGER_H

#include "callitem.h"
#include "modem_interface.h"
#include <QtDBus>
#include <QDebug>

#define OFONO_SERVICE "org.ofono"
#define OFONO_MANAGER_PATH "/"

#define DEFAULT_CLIR "default"

class CallManager: public org::ofono::VoiceCallManager
{
    Q_OBJECT

    Q_PROPERTY(QList<CallItem *> calls READ calls)
    Q_PROPERTY(QList<CallItem *> multipartyCalls READ multipartyCalls)

public:
    CallManager(const QString &modemPath);
    virtual ~CallManager();
    bool isValid();

    QList<CallItem *> calls() const;
    QList<CallItem *> multipartyCalls() const;
    CallItem *activeCall() const;

    QStringList dumpProperties();

public Q_SLOTS:
    void setActiveCall(const CallItem &call);

    // Initiate a new outgoing call
    void dial(const PeopleItem *person);
    void dial(const QString number);

    // Swap Active and Held calls (held becomes active, active becomes held)
    void swapCalls();

    // Releases all calls.
    void hangupAll();

    // Put the current call (including multi-party calls) on hold
    // and answer the currently waiting call.
    void holdAndAnswer();

    /*
     * TODO: Implement remaining Ofono APIs:
     *

    // Join Active and Held calls and disconnects from both calls
    void transferCalls();

    // Release currently active call and answer the currently waiting call
    releaseAndAnswer()

    // Place the multi-party call on hold and makes desired call active
    privateChat(CallItem)

    // Join active and held calls together into a multi-party call
    createMultipartyCall()

    // Hang up the multi-party call (all participating calls released)
    hangupMultipartyCall()

    // Sends the DTMF tones to the network
    sendTones()

     */

Q_SIGNALS:
    void callsChanged();
    void incomingCall(CallItem *call);
    void incomingCall(QString path);
    void callDisconnected(const CallItem &call);
    void connected();
    void disconnected();

private Q_SLOTS:
    void updateCallItems();
    void updateMultipartyCallItems();
    void setCalls(QList<QDBusObjectPath> calls);
    void setMultipartyCalls(QList<QDBusObjectPath> calls);
    void getPropertiesFinished(QDBusPendingCallWatcher *watcher);
    void dialFinished(QDBusPendingCallWatcher *watcher);
    void hangupAllFinished(QDBusPendingCallWatcher *watcher);
    void swapFinished(QDBusPendingCallWatcher *watcher);
    void holdAndAnswerFinished(QDBusPendingCallWatcher *watcher);
    void propertyChanged(const QString &in0, const QDBusVariant &in1);
    void callStateChanged();

private:
    QStringList        m_properties;
    QList<QString>     m_calls;
    QList<CallItem *>  m_callItems;
    QList<QString>     m_multipartyCalls;
    QList<CallItem *>  m_multipartyCallItems;
    CallItem          *m_activeCall;
    bool               m_connected;

    Q_DISABLE_COPY(CallManager)
};

#endif // CALLMANAGER_H
