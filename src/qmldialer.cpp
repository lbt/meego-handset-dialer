/*
 * dialer - Declarative Dialer UX Main Window.
 * Copyright (c) 2011, Tom Swindell.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "common.h"
#include "qmldialer.h"

#include "callmanager.h"
#include "managerproxy.h"

class QMLDialerPrivate
{
public:
    QMLDialerPrivate()
        : currentCall(NULL)
    { TRACE }

    QMLCallItem *currentCall;
};

QMLDialer::QMLDialer(QObject *parent)
    : QObject(parent), d(new QMLDialerPrivate)
{
    TRACE
    CallManager *cm = ManagerProxy::instance()->callManager();

    this->connectAll();

    if(cm->activeCall()) d->currentCall = new QMLCallItem(cm->activeCall(), this);
}

QMLDialer::~QMLDialer()
{
    TRACE
    delete this->d;
}

QString QMLDialer::mailbox() const
{
    TRACE
    return ManagerProxy::instance()->voicemail()->mailbox();
}

void QMLDialer::setMailbox(const QString &number)
{
    TRACE
    ManagerProxy::instance()->voicemail()->setMailbox(number);
}

QMLCallItem* QMLDialer::currentCall() const
{
    TRACE
    return d->currentCall;
}

void QMLDialer::dial(const QString &msisdn)
{
    TRACE
    ManagerProxy::instance()->callManager()->dial(msisdn);
}

void QMLDialer::hangupAll()
{
    TRACE
    ManagerProxy::instance()->callManager()->hangupAll();
}

void QMLDialer::sendTones(const QString &tones)
{
    TRACE
    ManagerProxy::instance()->callManager()->sendTones(tones);
}

void QMLDialer::transfer()
{
    TRACE
    ManagerProxy::instance()->callManager()->transferCalls();
}

void QMLDialer::swapCalls()
{
    TRACE
    ManagerProxy::instance()->callManager()->swapCalls();
}

void QMLDialer::releaseAndAnswer()
{
    TRACE
    ManagerProxy::instance()->callManager()->releaseAndAnswer();
}

void QMLDialer::holdAndAnswer()
{
    TRACE
    ManagerProxy::instance()->callManager()->holdAndAnswer();
}

void QMLDialer::createMultiparty()
{
    TRACE
    ManagerProxy::instance()->callManager()->createMultipartyCall();
}

void QMLDialer::hangupMultiparty()
{
    TRACE
    ManagerProxy::instance()->callManager()->HangupMultiparty();
}

void QMLDialer::privateChat(const QMLCallItem &call)
{
    TRACE
    ManagerProxy::instance()->callManager()->privateChat(*call.proxy());
}

void QMLDialer::onCallsChanged()
{
    TRACE
    CallManager *cm = ManagerProxy::instance()->callManager();

    if(cm->activeCall())
    {
        this->onIncomingCall(cm->activeCall());
    }
    else
    {
        d->currentCall = NULL;
    }
}

void QMLDialer::onIncomingCall(CallItem *callitem)
{
    TRACE

    d->currentCall = new QMLCallItem(callitem, this);
    emit this->incomingCall();
}

void QMLDialer::connectAll()
{
    TRACE
    CallManager *cm = ManagerProxy::instance()->callManager();

    QObject::connect(cm, SIGNAL(callsChanged()), SLOT(onCallsChanged()));
    QObject::connect(cm, SIGNAL(incomingCall(CallItem*)), SLOT(onIncomingCall(CallItem*)));
}
