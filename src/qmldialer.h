/*
 * dialer - Declarative Dialer Adapter.
 * Copyright (c) 2011, Tom Swindell.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef QMLDIALER_H
#define QMLDIALER_H

#include <QObject>

#include "qmlcallitem.h"

class QMLDialer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QMLCallItem* currentCall READ currentCall)

public:
    explicit QMLDialer(QObject *parent = 0);
            ~QMLDialer();

    QMLCallItem* currentCall    () const;

Q_SIGNALS:
    void incomingCall();

public Q_SLOTS:
    void dial(const QString &msisdn);
    void hangupAll();

    void sendTones(const QString &tones);

    void transfer();
    void swapCalls();
    void releaseAndAnswer();
    void holdAndAnswer();

    void createMultiparty();
    void hangupMultiparty();
    void privateChat(const QMLCallItem &callitem);

protected Q_SLOTS:
    void connectAll();

    void onCallsChanged();
    void onIncomingCall(CallItem *callitem);

private:
    class QMLDialerPrivate *d;
};

#endif // QMLDIALER_H
