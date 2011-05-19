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
#include "qmlcallitem.h"

class QMLCallItemPrivate
{
public:
    CallItem *proxy;
};

QMLCallItem::QMLCallItem(CallItem *proxy, QObject *parent)
    : QObject(parent), d(new QMLCallItemPrivate)
{
    TRACE
    d->proxy = proxy;
    QObject::connect(proxy->callProxy(), SIGNAL(stateChanged()), SLOT(onStateChanged()));
}

QMLCallItem::~QMLCallItem()
{
    TRACE
    delete this->d;
}

CallItem* QMLCallItem::proxy() const
{
    TRACE
    return d->proxy;
}

QString QMLCallItem::msisdn() const
{
    TRACE
    return d->proxy->callProxy()->lineID();
}

QString QMLCallItem::name() const
{
    TRACE
    return d->proxy->callProxy()->name();
}

QString QMLCallItem::state() const
{
    TRACE
    return d->proxy->callProxy()->state();
}

QString QMLCallItem::reason() const
{
    TRACE
    return d->proxy->callProxy()->reason();
}

QDateTime QMLCallItem::startedAt() const
{
    TRACE
    return d->proxy->callProxy()->startTime();
}

int QMLCallItem::duration() const
{
    TRACE
    return d->proxy->callProxy()->duration();
}

bool QMLCallItem::isMultiparty() const
{
    TRACE
    return d->proxy->callProxy()->multiparty();
}

void QMLCallItem::answer()
{
    TRACE
    d->proxy->callProxy()->answer();
}

void QMLCallItem::deflect(const QString &msisdn)
{
    TRACE
    d->proxy->callProxy()->deflect(msisdn);
}

void QMLCallItem::hangup()
{
    TRACE
    d->proxy->callProxy()->hangup();
}

void QMLCallItem::onStateChanged()
{
    TRACE
    emit this->stateChanged(d->proxy->callProxy()->state());
}
