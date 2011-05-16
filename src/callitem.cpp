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
#include "dialerapplication.h"
#include "seasidesyncmodel.h"
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

    QString l_ringtoneFile = QString("%1/%2/stereo/%3")
                                     .arg(SOUNDS_DIR)
                                     .arg(MTheme::instance()->currentTheme())
                                     .arg(DEFAULT_RINGTONE);
    QString l_rtKeyValue = m_rtKey->value(QVariant(l_ringtoneFile)).toString();

    if (QFileInfo(l_rtKeyValue).exists()) {
        m_ringtonefile = l_ringtoneFile;
        qDebug() << QString("CallItem: %1 using ringtone: %2")
                           .arg(m_path)
                           .arg(m_ringtonefile);
    } else {
        qWarning() << QString("CallItem: %1 ringtone not found: %2")
                           .arg(m_path)
                           .arg(l_rtKeyValue);
    }

    m_ringtone->setMedia(QMediaContent(QUrl::fromLocalFile(m_ringtonefile)));
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

    if (m_peopleItem)
        delete m_peopleItem;
    m_peopleItem = 0;

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
            connect(call,SIGNAL(dataChanged()),this,SLOT(callDataChanged()));
            connect(call,SIGNAL(multipartyChanged()),this,SLOT(callMultipartyChanged()));
        } else
            qCritical("Invalid CallProxy instance!");
    } else
        qCritical("Empty call path.  Can not create CallProxy!");

    populatePeopleItem();

    if (state() == CallItemModel::STATE_INCOMING ||
        state() == CallItemModel::STATE_WAITING)
    {
        // Start ringing
        if (!m_isconnected && m_ringtone) {
           connect(m_ringtone, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                               SLOT(ringtoneStatusChanged(QMediaPlayer::MediaStatus)));
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

QString CallItem::name() const
{
    TRACE
    return (isValid())?model()->name():QString();
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
            connect(m_ringtone, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                                SLOT(ringtoneStatusChanged(QMediaPlayer::MediaStatus)));
            m_isconnected = TRUE;
            m_ringtone->play();
        }
    } else {
        // Stop ringing
        if (m_ringtone) {
            disconnect(m_ringtone, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
            m_isconnected = FALSE;
            m_ringtone->stop();
        }
    }
    emit stateChanged();
}

void CallItem::callDataChanged()
{
    TRACE
    populatePeopleItem();
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

void CallItem::populatePeopleItem()
{
    TRACE

    QModelIndexList matches;
    matches.clear();
    int role = Seaside::SearchRole;
    int hits = -1;

    //% "Unknown Caller"
    QString pi_name   = qtTrId("xx_unknown_caller");
    QString pi_photo  = "icon-m-content-avatar-placeholder";
    //% "Private"
    QString pi_lineid = qtTrId("xx_private");

    if (!lineID().isEmpty()) {
        pi_lineid = stripLineID(lineID());
        SeasideSyncModel *contacts = DA_SEASIDEMODEL;
        QModelIndex first = contacts->index(0,Seaside::ColumnPhoneNumbers);
        matches = contacts->match(first, role, QVariant(pi_lineid), hits);

        QString firstName = QString();
        QString lastName = QString();

        if (!matches.isEmpty()) {
            QModelIndex person = matches.at(0); //First match is all we look at
            SEASIDE_SHORTCUTS
            SEASIDE_SET_MODEL_AND_ROW(person.model(), person.row());

            firstName = SEASIDE_FIELD(FirstName, String);
            lastName = SEASIDE_FIELD(LastName, String);
            pi_photo = SEASIDE_FIELD(Avatar, String);
        } else if (!name().isEmpty()) {
            // We don't have a contact, but we have a callerid name, let's use it
            firstName = name();
        }

        if (lastName.isEmpty() && !firstName.isEmpty())
            // Contacts first (common) name
            //% "%1"
            pi_name = qtTrId("xx_first_name").arg(firstName);
        else if (firstName.isEmpty() && !lastName.isEmpty())
            // BMC# 8079 - NW
            // Contacts last (sur) name
            //% "%1"
            pi_name = qtTrId("xx_last_name").arg(lastName);
        else if (!firstName.isEmpty() && !lastName.isEmpty())
            // Contacts full, sortable name, is "Firstname Lastname"
            //% "%1 %2"
            pi_name = qtTrId("xx_first_last_name").arg(firstName)
                .arg(lastName);

    } else {
        //% "Unavailable"
        pi_lineid = qtTrId("xx_unavailable");
    }

    if (m_peopleItem != NULL)
        delete m_peopleItem;
    m_peopleItem = new PeopleItem();

    m_peopleItem->setName(pi_name);
    m_peopleItem->setPhoto(pi_photo);
    m_peopleItem->setPhone(pi_lineid);
}

void CallItem::ringtoneStatusChanged(QMediaPlayer::MediaStatus status)
{
    TRACE
    if (status == QMediaPlayer::EndOfMedia)
    {
      m_ringtone->setMedia(QMediaContent(QUrl::fromLocalFile(m_ringtonefile)));
      m_ringtone->play();
    }
}

bool CallItem::multiparty()
{
    TRACE
    return (isValid())?model()->multiparty():false;
}

void CallItem::callMultipartyChanged()
{
    TRACE
    emit multipartyChanged();
}
