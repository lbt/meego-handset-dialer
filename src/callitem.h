/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef CALLITEM_H
#define CALLITEM_H

#include "callitemmodel.h"
#include "peopleitem.h"
#include <QString>
#include <QDateTime>
#include <QtDBus>
#include <QMediaPlayer>
#include <MGConfItem>
#include <MWidgetController>

class CallItem: public MWidgetController
{
    Q_OBJECT
    M_CONTROLLER(CallItem)

    Q_PROPERTY(QString path READ path WRITE setPath)
    Q_PROPERTY(QString lineID READ lineID)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(CallItemModel::CallState state READ state)
    Q_PROPERTY(CallItemModel::CallDirection direction READ direction WRITE setDirection)
    Q_PROPERTY(CallItemModel::CallDisconnectReason reason READ reason)
    Q_PROPERTY(int duration READ duration)
    Q_PROPERTY(QDateTime startTime READ startTime)
    Q_PROPERTY(bool multiparty READ multiparty)

    Q_PROPERTY(PeopleItem* peopleItem READ peopleItem WRITE setPeopleItem)
    Q_PROPERTY(CallProxy* callProxy READ callProxy)

public:
    CallItem(const QString path = QString(), MWidget *parent = 0);
    virtual ~CallItem();

    QString path() const;
    QString lineID() const;
    QString name() const;
    CallItemModel::CallState state() const;
    CallItemModel::CallDirection direction() const;
    CallItemModel::CallDisconnectReason reason() const;
    int duration() const;
    QDateTime startTime() const;
    PeopleItem *peopleItem() const;
    CallProxy *callProxy() const;
    bool isValid();
    bool isValid() const;
    bool multiparty();

public Q_SLOTS:
    void init();
    void setPeopleItem(PeopleItem *person);
    bool setPath(QString path);  // Setting this will create the CallProxy
    void setDirection(CallItemModel::CallDirection direction);
    void click();

Q_SIGNALS:
    // TODO: handle tap-and-hold
    void clicked();
    void stateChanged();
    void dataChanged();
    void multiPartyChanged();

private Q_SLOTS:
    void callStateChanged();
    void callDataChanged();
    void callDisconnected(const QString &reason);
    void ringtoneStatusChanged(QMediaPlayer::MediaStatus status);
    void callMultiPartyChanged();

private:
    QVariant itemChange(GraphicsItemChange change, const QVariant &val);
    void populatePeopleItem();

    QString               m_path;
    PeopleItem           *m_peopleItem;
    QMediaPlayer         *m_ringtone;
    MGConfItem           *m_rtKey;
    bool                  m_isconnected;
    QString               m_ringtonefile;

    Q_DISABLE_COPY(CallItem)
};

#endif // CALLITEM_H
