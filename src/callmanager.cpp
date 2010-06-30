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
#include "callmanager.h"

CallManager::CallManager(const QString &modemPath)
    : org::ofono::VoiceCallManager(OFONO_SERVICE,
                                   modemPath,
                                   QDBusConnection::systemBus()),
      m_activeCall(0),
      m_connected(false)
{
    TRACE

    if (!org::ofono::VoiceCallManager::isValid())
        qCritical() << QString("Failed to connect to %1 on modem %2:\n\t%3")
                       .arg(staticInterfaceName())
                       .arg(modemPath)
                       .arg(lastError().message());
    else {
        QDBusPendingReply<QVariantMap> reply;
        QDBusPendingCallWatcher *watcher;

        reply = GetProperties();
        watcher = new QDBusPendingCallWatcher(reply);

        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                         SLOT(getPropertiesFinished(QDBusPendingCallWatcher*)));
        connect(this, SIGNAL(PropertyChanged(const QString&, const QDBusVariant&)),
                SLOT(propertyChanged(const QString&, const QDBusVariant&)));
    }
}

CallManager::~CallManager()
{
    TRACE
    // FIXME: Do something here!!!
}

bool CallManager::isValid()
{
    TRACE
    return (org::ofono::VoiceCallManager::isValid() && m_connected);
}

QList<CallItem *> CallManager::calls() const
{
    TRACE
    return m_callItems;
}

QList<CallItem *> CallManager::multipartyCalls() const
{
    TRACE
    return m_multipartyCallItems;
}

CallItem *CallManager::activeCall() const
{
    TRACE
    if (m_callItems.size())
    foreach (CallItem *c, m_callItems)
        if (c->state() == CallItemModel::STATE_ACTIVE)
            return c;
    return NULL;
}

void CallManager::setActiveCall(const CallItem &call)
{
    TRACE
    if (!call.isActive())
        swapCalls();
}

void CallManager::dial(const PeopleItem *person)
{
    TRACE
    dial(person->phone());
}

void CallManager::dial(const QString number)
{
    TRACE

    QDBusPendingReply<QDBusObjectPath> reply;
    QDBusPendingCallWatcher *watcher;

    reply = Dial(stripLineID(number), QString());
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(dialFinished(QDBusPendingCallWatcher*)));
}

void CallManager::swapCalls()
{
    TRACE

    QDBusPendingReply<> reply;
    QDBusPendingCallWatcher *watcher;

    reply = SwapCalls();
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(swapFinished(QDBusPendingCallWatcher*)));
}

void CallManager::hangupAll()
{
    TRACE

    QDBusPendingReply<QDBusObjectPath> reply;
    QDBusPendingCallWatcher *watcher;

    reply = HangupAll();
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(hangupAllFinished(QDBusPendingCallWatcher*)));
}

void CallManager::holdAndAnswer()
{
    TRACE

    QDBusPendingReply<> reply;
    QDBusPendingCallWatcher *watcher;

    reply = HoldAndAnswer();
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(holdAndAnswerFinished(QDBusPendingCallWatcher*)));
}

/*
 * TODO: Implement remaining Ofono APIs:
 *
void CallManager::transferCalls()
{
    TRACE
}

void CallManager::releaseAndAnswer()
{
    TRACE
}

void CallManager::privateChat(CallItem)
{
    TRACE
}

void CallManager::createMultipartyCall()
{
    TRACE
}

void CallManager::hangupMultipartyCall()
{
    TRACE
}

void CallManager::sendTones()
{
    TRACE
}

 */

/*
 * Private slots for DBus async replies
 */

void CallManager::updateCallItems()
{
    TRACE
    bool changed = false;

    // If ofono call list is empty (no calls), empty our CallItem list too.
    if (m_calls.isEmpty() && !m_callItems.isEmpty()) {
        qDebug() << QString("Purging all CallItems");
        foreach (CallItem *item, m_callItems) {
            disconnect(item, SIGNAL(stateChanged()));
            delete item;
        }
        m_callItems.clear();
        emit callsChanged();
        return;
    }

    // Remove CallItems that are not in the ofono "calls" list
    QMutableListIterator<CallItem*> iter(m_callItems);
    while (iter.hasNext()) {
        CallItem *item = iter.next();
        // This item is not in the ofono list, remove it
        if (!m_calls.contains(item->path())) {
            qDebug() << QString("Removing old CallItem %1").arg(item->path());
            disconnect(item, SIGNAL(stateChanged()));
            delete item;
            iter.remove();
            changed = true;
        }
    }

    // Insert new CallItems for paths in the ofono "calls" list we are missing
    foreach (QString callPath, m_calls) {
        bool matchFound = false;
        foreach (CallItem *item, m_callItems) {
            // This call is not in our CallItem list, insert it
            if (item->path() == callPath) {
                matchFound = true;
                break;
            }
        }
        // Insert a new CallItem
        if (!matchFound) {
            qDebug() << QString("Inserting new CallItem %1").arg(callPath);
            CallItem *call = new CallItem(callPath);
            connect (call, SIGNAL(stateChanged()), SLOT(callStateChanged()));
            m_callItems << call;
            changed = true;

            // NOTE: Must explicity bubble this up since incoming and waiting
            //       calls do not "changeState" unless they are handled or
            //       timeout
            if (call->state() == CallItemModel::STATE_INCOMING)
                emit incomingCall(call);
            else if (call->state() == CallItemModel::STATE_WAITING)
                emit incomingCall(call);
        }
    }

    if (changed)
        emit callsChanged();
}

void CallManager::updateMultipartyCallItems()
{
    TRACE

    // If ofono multiparty call list is empty (no calls), empty our
    // multiparty CallItem list too.
    if (m_multipartyCalls.isEmpty() && !m_multipartyCallItems.isEmpty()) {
        qDebug() << QString("Purging all multiparty CallItems");
        foreach (CallItem *item, m_multipartyCallItems) delete item;
        m_multipartyCallItems.clear();
        return;
    }

    // Remove CallItems that are not in the ofono "calls" list
    QMutableListIterator<CallItem*> iter(m_multipartyCallItems);
    while (iter.hasNext()) {
        CallItem *item = iter.next();
        // This item is not in the ofono list, remove it
        if (!m_multipartyCalls.contains(item->path())) {
            qDebug() << QString("Removing old multiparty CallItem %1")
                        .arg(item->path());
            delete item;
            iter.remove();
        }
    }

    // Insert new CallItems for paths in the ofono "calls" list we are missing
    foreach (QString callPath, m_multipartyCalls) {
        bool matchFound = false;
        foreach (CallItem *item, m_multipartyCallItems) {
            // This call is not in our CallItem list, insert it
            if (item->path() == callPath) {
                matchFound = true;
                break;
            }
        }
        // Insert a new CallItem
        if (!matchFound) {
            m_multipartyCallItems << new CallItem(callPath);
            qDebug() << QString("Inserting new multiparty CallItem %1")
                        .arg(callPath);
        }
    }
}

void CallManager::setCalls(QList<QDBusObjectPath> calls)
{
    TRACE

    m_calls.clear();

    foreach (QDBusObjectPath c, calls)
        m_calls << QString(c.path());

    updateCallItems();
}

void CallManager::setMultipartyCalls(QList<QDBusObjectPath> calls)
{
    TRACE

    m_multipartyCalls.clear();

    foreach (QDBusObjectPath c, calls)
        m_multipartyCalls << QString(c.path());

    updateMultipartyCallItems();
}

void CallManager::getPropertiesFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE

    QDBusPendingReply<QVariantMap> reply = *watcher;

    if (reply.isError()) {
        qCritical() << QString("Failed to connect to %1 on modem %2:\n\t%3")
                       .arg(staticInterfaceName())
                       .arg(path())
                       .arg(lastError().message());
        return;
    }

    QVariantMap props = reply.value();

    QList<QDBusObjectPath> calls, mpcalls;

    calls   = qdbus_cast<QList<QDBusObjectPath> >(props["Calls"]);
    mpcalls = qdbus_cast<QList<QDBusObjectPath> >(props["MultipartyCalls"]);

    setCalls(calls);
    setMultipartyCalls(mpcalls);

    // Indicate for this instance, that we've actually performed at least
    // one round trip call to this VoiceCallManager and we are in sync with it
    // First sucessfull GetProperties == connected
    if (!m_connected) {
        m_connected = true;
        emit connected();
    }
}

void CallManager::dialFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE

    QDBusPendingReply<QDBusObjectPath> reply = *watcher;

    if (reply.isError()) {
        qCritical() << QString("Dial() Failed: %1 - %2")
                       .arg(reply.error().name())
                       .arg(reply.error().message());
        return;
    } else {
        QDBusObjectPath val = reply.value();
        qDebug() << QString("Dial() Success: path == %1").arg(val.path());
    }
}

void CallManager::hangupAllFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE
    Q_UNUSED(watcher)
}

void CallManager::swapFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE
    QDBusPendingReply<> reply = *watcher;

    if (reply.isError())
        qCritical() << QString("SwapCalls() Failed: %1 - %2")
                       .arg(reply.error().name())
                       .arg(reply.error().message());
}

void CallManager::holdAndAnswerFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE
    QDBusPendingReply<> reply = *watcher;

    if (reply.isError())
        qCritical() << QString("HoldAndAnswer() Failed: %1 - %2")
                       .arg(reply.error().name())
                       .arg(reply.error().message());
}

void CallManager::propertyChanged(const QString &in0, const QDBusVariant &in1)
{
    TRACE
    Q_UNUSED(in1)
    qDebug() << QString("Property \"%1\" changed...").arg(in0);
    if (in0 == "Calls") {
        QList<QDBusObjectPath> calls;
        calls = qdbus_cast<QList<QDBusObjectPath> >(in1.variant());
        setCalls(calls);
    } else if (in0 == "MultipartyCalls") {
        QList<QDBusObjectPath> calls;
        calls = qdbus_cast<QList<QDBusObjectPath> >(in1.variant());
        setMultipartyCalls(calls);
    } else if (in0 == "EmergencyNumbers") {
        qDebug() << QString("TODO: Handle EmergencyNumber...");
    } else
        qDebug() << QString("Unexpected property changed...");
}

void CallManager::callStateChanged()
{
    CallItem *call = dynamic_cast<CallItem *>(sender());
    qDebug() << QString("%1 (%2) state has changed to %3")
                .arg(call->path())
                .arg(call->lineID())
                .arg(call->state());
    emit callsChanged();
}

QStringList CallManager::dumpProperties()
{
    m_properties.clear();

    // Single party calls
    m_properties << "<ul><li>Calls:</li>";
    if (m_callItems.size())
        foreach (CallItem *c, m_callItems) {
            m_properties << QString("<ul><li>Path: %1</li>").arg(c->path());
            m_properties << QString("<li>LineID  : %1</li>").arg(c->lineID());
            m_properties << QString("<li>State   : %1</li>").arg(c->state());
            m_properties << QString("<li>Started : %1</li>")
                            .arg(c->startTime().toString());
            if (c->state() == CallItemModel::STATE_DISCONNECTED)
                m_properties << QString("<li>Reason: %1</li></ul></ul>")
                                .arg(c->reason());
            else
                m_properties << QString("</ul></ul>");
        }
    else
        m_properties << "<ul><li>None</li></ul></ul>";

    // Multi party calls
    m_properties << "<ul><li>Multiparty Calls:</li>";
    if (m_multipartyCallItems.size())
        foreach (CallItem *c, m_multipartyCallItems) {
            m_properties << QString("<ul><li>Path: %1</li>").arg(c->path());
            if (c->state() == CallItemModel::STATE_DISCONNECTED)
                m_properties << QString("<li>Reason: %1</li></ul></ul>")
                                .arg(c->reason());
            else
                m_properties << QString("</ul></ul>");
        }
    else
        m_properties << "<ul><li>None</li></ul></ul>";

    return m_properties;
}
