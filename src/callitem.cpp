/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "common.h"
#include "callitem.h"
#include "callitemmodel.h"
#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QDebug>
#include <MTheme>

#include <MWidgetCreator>

#define DEFAULT_RINGTONE "ring-1.wav"

M_REGISTER_WIDGET(CallItem)

CallItem::CallItem(const QString path, MWidget *parent)
    : MWidgetController(new CallItemModel, parent),
      m_path(path),
      m_peopleItem(NULL),
      m_ringtone(new QMediaPlayer()),
      m_rtKey(new MGConfItem("/apps/dialer/defaultRingtone")),
      m_isconnected(FALSE),
      m_ringtonefile("")
{
    TRACE

    m_ringtonefile = QString("%1/%2/stereo/%3")
                                     .arg(SOUNDS_DIR)
                                     .arg(MTheme::instance()->currentTheme())
                                     .arg(DEFAULT_RINGTONE);
    m_ringtone->setMedia(QMediaContent(QUrl::fromLocalFile(
                m_rtKey->value(QVariant(m_ringtonefile)).toString())));
    m_ringtone->setVolume(100);

    if (isValid())
        init();
}

CallItem::~CallItem()
{
    TRACE
    if (m_ringtone) {
        disconnect(m_ringtone, SIGNAL(positionChanged(qint64)));
        m_ringtone->stop();
        delete m_ringtone;
        m_ringtone = 0;
    }

    if (m_rtKey)
        delete m_rtKey;
    m_rtKey = 0;

    // delete the callproxy object
    if (callProxy())
        delete callProxy();
}

void CallItem::init()
{
    TRACE
    if (!m_path.isEmpty()) {
        CallProxy *call = new CallProxy(m_path);

        if (call->isValid()) {
            model()->setCall(call);
            connect(call,SIGNAL(stateChanged()),this,SLOT(callStateChanged()));
        } else
            qCritical("Invalid CallProxy instance!");
    } else
        qCritical("Empty call path.  Can not create CallProxy!");

    if (state() == CallItemModel::STATE_INCOMING ||
        state() == CallItemModel::STATE_WAITING)
    {
        // Start ringing
        if (!m_isconnected && m_ringtone) {
           connect(m_ringtone, SIGNAL(stateChanged(QMediaPlayer::State)),
                               SLOT(ringtoneStateChanged(QMediaPlayer::State)));
           m_isconnected = TRUE;
           m_ringtone->play();
        }
    }
}

bool CallItem::isValid()
{
    TRACE
    return (!path().isEmpty());
}

bool CallItem::isValid() const
{
    TRACE
    return (!path().isEmpty());
}

QString CallItem::path() const
{
    TRACE
    return m_path;
}

bool CallItem::setPath(QString path)
{
    TRACE
    if (!m_path.isEmpty()) {
        qCritical("Path already set and can not be changed once it is set");
        return false;
    } else if (path.isEmpty()) {
        qCritical("It makes no sense to set Path to an empty string!?!?");
        return false;
    }

    m_path = path;

    init();

    return true;
}

void CallItem::setDirection(CallItemModel::CallDirection direction)
{
    TRACE
    model()->setDirection(direction);
}

QString CallItem::lineID() const
{
    TRACE
    return (isValid())?model()->lineID():QString();
}

CallItemModel::CallState CallItem::state() const
{
    TRACE
    return model()->stateType();
}

CallItemModel::CallDirection CallItem::direction() const
{
    TRACE
    return model()->direction();
}

CallItemModel::CallDisconnectReason CallItem::reason() const
{
    TRACE
    return model()->reasonType();
}

int CallItem::duration() const
{
    TRACE
    return model()->duration();
}

QDateTime CallItem::startTime() const
{
    TRACE
    return model()->startTime();
}

PeopleItem * CallItem::peopleItem() const
{
    TRACE
    return m_peopleItem;
}

CallProxy* CallItem::callProxy() const
{
    TRACE
    return model()->call();
}

void CallItem::setPeopleItem(PeopleItem *person)
{
    TRACE
    if (m_peopleItem)
        delete m_peopleItem;
    m_peopleItem = person;
}

void CallItem::click()
{
    TRACE

    emit clicked();
}

void CallItem::callStateChanged()
{
    TRACE
    if (state() == CallItemModel::STATE_INCOMING ||
        state() == CallItemModel::STATE_WAITING)
    {
        // Start ringing
        if (!m_isconnected && m_ringtone) {
            connect(m_ringtone, SIGNAL(stateChanged(QMediaPlayer::State)),
                                SLOT(ringtoneStateChanged(QMediaPlayer::State)));
            m_isconnected = TRUE;
            m_ringtone->play();
        }
    } else {
        // Stop ringing
        if (m_ringtone) {
            disconnect(m_ringtone, SIGNAL(stateChanged(QMediaPlayer::State)));
            m_isconnected = FALSE;
            m_ringtone->stop();
        }
    }
    emit stateChanged();
}

void CallItem::callDisconnected(const QString &reason)
{
    TRACE
    Q_UNUSED(reason);
}

QVariant CallItem::itemChange(GraphicsItemChange change, const QVariant &val)
{
    TRACE
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        model()->setSelected(val.toBool());

    return QGraphicsItem::itemChange(change, val);
}

void CallItem::ringtoneStateChanged(QMediaPlayer::State state)
{
    TRACE
    if (state != QMediaPlayer::PlayingState)
    {
      m_ringtone->setMedia(QMediaContent(QUrl::fromLocalFile(m_ringtonefile)));
      m_ringtone->play();
    }
}
