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
#include "callproxy.h"
#include "managerproxy.h"

CallProxy::CallProxy(const QString &callPath)
    : org::ofono::VoiceCall(OFONO_SERVICE,
                            callPath,
                            QDBusConnection::systemBus()),
      m_lineid(QString()),
      m_state(QString()),
      m_startTime(QDateTime()),
      m_reason(QString()),
      m_connected(false)
{
    TRACE

    if (!org::ofono::VoiceCall::isValid())
        qCritical() << QString("Failed to connect to %1 for call %2:\n\t%3")
                       .arg(staticInterfaceName())
                       .arg(callPath)
                       .arg(lastError().message());
    else {
        QDBusPendingReply<QVariantMap> reply;
        QDBusPendingCallWatcher *watcher;

        reply = GetProperties();
        watcher = new QDBusPendingCallWatcher(reply);

/*
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                         SLOT(getPropertiesFinished(QDBusPendingCallWatcher*)));
*/

        // Force this to be sync to ensure we have initial properties
        watcher->waitForFinished();
        getPropertiesFinished(watcher);

        if (isValid()) {
            connect(this,
                    SIGNAL(PropertyChanged(const QString&,const QDBusVariant&)),
                    SLOT(propertyChanged(const QString&,const QDBusVariant&)));
            connect(this, SIGNAL(DisconnectReason(const QString&)),
                          SLOT(disconnectReason(const QString&)));
        } else
            qCritical() << QString("Invalid CallProxy instance: state == %1")
                           .arg(m_state);
    }
}

CallProxy::~CallProxy()
{
    TRACE
    // FIXME: Do something here!!!
}

bool CallProxy::isValid()
{
    TRACE
    return (org::ofono::VoiceCall::isValid() &&
            m_connected &&
            (m_state != "disconnected"));
}

QString CallProxy::lineID() const
{
    TRACE
    return m_lineid;
}

QString CallProxy::state() const
{
    TRACE
    return m_state;
}

QDateTime CallProxy::startTime() const
{
    TRACE
    return m_startTime;
}

int CallProxy::duration() const
{
    TRACE
    return m_startTime.time().elapsed();
}

QString CallProxy::reason() const
{
    TRACE
    return m_reason;
}

void CallProxy::answer()
{
    TRACE

    ResourceProxy *resource = ManagerProxy::instance()->resource();

    connect(resource, SIGNAL(answerResourceAcquired()), SLOT(proceedCallAnswer()));
    connect(resource, SIGNAL(answerResourceDenied()), SLOT(deniedCallAnswer()));

    resource->acquireAnswerResource();
}

void CallProxy::proceedCallAnswer()
{
    TRACE

    ResourceProxy *resource = ManagerProxy::instance()->resource();
    QDBusPendingReply<QDBusObjectPath> reply;
    QDBusPendingCallWatcher *watcher;

    disconnect(resource, SIGNAL(answerResourceAcquired()));
    disconnect(resource, SIGNAL(answerResourceDenied()));

    reply = Answer();
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(answerFinished(QDBusPendingCallWatcher*)));
}

void CallProxy::deniedCallAnswer()
{
    TRACE

    ResourceProxy *resource = ManagerProxy::instance()->resource();

    disconnect(resource, SIGNAL(answerResourceAcquired()));
    disconnect(resource, SIGNAL(answerResourceDenied()));

    // Hang up the incoming call, if resources to accept it are inavailabe
    hangup();

    emit ManagerProxy::instance()->callManager()->deniedCallAnswer();
}

void CallProxy::deflect(const QString toNumber)
{
    TRACE

    QDBusPendingReply<QDBusObjectPath> reply;
    QDBusPendingCallWatcher *watcher;

    reply = Deflect(toNumber);
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(deflectFinished(QDBusPendingCallWatcher*)));
}

void CallProxy::hangup()
{
    TRACE

    QDBusPendingReply<> reply;
    QDBusPendingCallWatcher *watcher;

    reply = Hangup();
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(hangupFinished(QDBusPendingCallWatcher*)));
}

void CallProxy::getPropertiesFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE

    QDBusPendingReply<QVariantMap> reply = *watcher;

    if (reply.isError()) {
        qCritical() << QString("Failed to connect to %1 for call %2:\n\t%3")
                       .arg(staticInterfaceName())
                       .arg(path())
                       .arg(lastError().message());
        return;
    }

    QVariantMap props = reply.value();

    QString l_start;

    m_lineid = qdbus_cast<QString>(props["LineIdentification"]);
    m_state  = qdbus_cast<QString>(props["State"]);
    l_start  = qdbus_cast<QString>(props["StartTime"]);

    setStartTimeFromString(l_start);

    // Indicate for this instance, that we've actually performed at least
    // one round trip call to this VoiceCall and we are in sync with it
    m_connected = true;
}

void CallProxy::answerFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;
    if (reply.isError())
        qCritical() << QString("Answer() Failed: %1 - %2")
                       .arg(reply.error().name())
                       .arg(reply.error().message());
}

void CallProxy::deflectFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;
    if (reply.isError())
        qCritical() << QString("Deflect() Failed: %1 - %2")
                       .arg(reply.error().name())
                       .arg(reply.error().message());
}

void CallProxy::hangupFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE
    QDBusPendingReply<> reply = *watcher;
    if (reply.isError())
        qCritical() << QString("Hangup() Failed: %1 - %2")
                       .arg(reply.error().name())
                       .arg(reply.error().message());
}

void CallProxy::propertyChanged(const QString &in0, const QDBusVariant &in1)
{
    TRACE

    if (in0 == "LineIdentification") {
        m_lineid = qdbus_cast<QString>(in1.variant());
    } else if (in0 == "State") {
        m_state  = qdbus_cast<QString>(in1.variant());
        emit stateChanged();
    } else if (in0 == "StartTime") {
        if (!m_startTime.isValid()) // No start time set yet
            setStartTimeFromString(qdbus_cast<QString>(in1.variant()));
    } else {
        qDebug() << QString("Unexpected property \"%1\" changed...").arg(in0);
    }
}

void CallProxy::disconnectReason(const QString &in0)
{
    TRACE
    m_reason = in0;
    emit callDisconnected(in0);
}

void CallProxy::setStartTimeFromString(const QString &val)
{
    TRACE
    if (val.isEmpty())
        return;

    // NOTE: QDateTime::fromString(val, Qt::ISODate) Fails since the date
    //       format from Ofono is in RFC 822 form, but QDateTime can't parse it
    // NOTE: Ofono formats time to string with the following format spec:
    //       %Y-%m-%dT%H:%M:%S%z
    struct tm time_tm;
    QByteArray  bytes = val.toAscii();
    const char *str = bytes.constData();
    if (strptime(str, "%Y-%m-%dT%H:%M:%S%z", &time_tm) != NULL) {
        time_t t = mktime(&time_tm);
        if (t >= (time_t)(0))
            m_startTime.setTime_t(t);
    }

    if (!m_startTime.isValid())
        qCritical() << QString("Format error, could not parse: %1").arg(str);
    else
        // FIXME: This could be a pre-existing call (i.e. a call is in progress
        //        when we are started), in which case this start is not really
        //        in sync with the *actuall* start time.
        m_startTime.time().start(); // begin elapsed time timer
}


QStringList CallProxy::dumpProperties()
{
    m_properties.clear();

    m_properties << QString("<ul><li>Path: %1</li>").arg(path());
    m_properties << QString("<li>LineID: %1</li>").arg(m_lineid);
    m_properties << QString("<li>State: %1</li>").arg(m_state);
    m_properties << QString("<li>StartTime: %1</li>")
                    .arg(m_startTime.toString());
    if (!m_reason.isEmpty())
        m_properties << QString("<li>DisconnectReason: %1</li></ul>")
                        .arg(m_reason);
    else
        m_properties << QString("</ul>");

    return m_properties;
}
