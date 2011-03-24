/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef CALLPROXY_H
#define CALLPROXY_H

#include "voicecall_interface.h"
#include <QtDBus>
#include <QDebug>

#define OFONO_SERVICE "org.ofono"
#define OFONO_MANAGER_PATH "/"

#define DEFAULT_CLIR "default"

class CallProxy: public org::ofono::VoiceCall
{
    Q_OBJECT

    Q_PROPERTY(QString   lineID READ lineID)
    Q_PROPERTY(QString   name READ name)
    Q_PROPERTY(QString   state READ state)
    Q_PROPERTY(QDateTime startTime READ startTime)
    Q_PROPERTY(int       duration READ duration)
    Q_PROPERTY(QString   reason READ reason)

public:
    CallProxy(const QString &callPath);
    virtual ~CallProxy();
    bool isValid();

    QString lineID() const;
    QString name() const;
    QString state() const;
    QDateTime startTime() const;
    int duration() const;
    QString reason() const;

    QStringList dumpProperties();

public Q_SLOTS:
    // Answers the incoming call
    // NOTE: only valid if state is incoming
    void answer();

    // Deflects the incoming or waiting call to number provided.
    // NOTE: only valid if state is incoming or waiting
    void deflect(const QString toNumber);

    // Hangs up the voice call
    void hangup();

Q_SIGNALS:
    void callDisconnected(const QString &reason);
    void stateChanged();
    void dataChanged();

private Q_SLOTS:
    // Slots to handle asyncronous DBus replies
    void getPropertiesFinished(QDBusPendingCallWatcher *watcher);
    void answerFinished(QDBusPendingCallWatcher *watcher);
    void deflectFinished(QDBusPendingCallWatcher *watcher);
    void hangupFinished(QDBusPendingCallWatcher *watcher);

    // Slots to handle DBus signals from ofono
    void propertyChanged(const QString &in0, const QDBusVariant &in1);
    void disconnectReason(const QString &in0);

    void proceedCallAnswer();
    void deniedCallAnswer();

private:
    void setStartTimeFromString(const QString &val);

private:
    QStringList        m_properties;
    QString            m_lineid;
    QString            m_name;
    QString            m_state;
    QDateTime          m_startTime;
    QString            m_reason;
    bool               m_connected;

    Q_DISABLE_COPY(CallProxy)
};

#endif // CALLPROXY_H
