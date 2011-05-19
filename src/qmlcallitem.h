/*
 * dialer - CallItem Declarative Proxy Implementation.
 * Copyright (c) 2011, Tom Swindell.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef QMLCALLITEM_H
#define QMLCALLITEM_H

#include "callitem.h"

#include <QObject>
#include <QDateTime>

class QMLCallItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString msisdn READ msisdn)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString reason READ reason)

    Q_PROPERTY(QDateTime startedAt READ startedAt)
    Q_PROPERTY(int duration READ duration)

    Q_PROPERTY(bool isMultiparty READ isMultiparty)

public:
    explicit QMLCallItem(CallItem *proxy, QObject *parent = 0);
            ~QMLCallItem();

    QString      msisdn         () const;
    QString      name           () const;
    QString      state          () const;

    QDateTime    startedAt      () const;
    int          duration       () const;

    QString      reason         () const;
    bool         isMultiparty   () const;

    CallItem*    proxy          () const;

Q_SIGNALS:
    void stateChanged   (const QString &state);

public Q_SLOTS:
    void answer         ();
    void deflect        (const QString &msisdn);
    void hangup         ();

protected Q_SLOTS:
    void onStateChanged ();

private:
    class QMLCallItemPrivate *d;

    Q_DISABLE_COPY(QMLCallItem)
};

#endif // QMLCALLITEM_H
