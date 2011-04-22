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
#include "managerproxy.h"

CallManager::CallManager(const QString &modemPath)
    : org::ofono::VoiceCallManager(OFONO_SERVICE,
                                   modemPath,
                                   QDBusConnection::systemBus()),
      m_connected(false)
{
    TRACE

    if (!org::ofono::VoiceCallManager::isValid()) {
        qCritical() << QString("Failed to connect to %1 on modem %2:\n\t%3")
                       .arg(staticInterfaceName())
                       .arg(modemPath)
                       .arg(lastError().message());
    } else {
        QDBusPendingReply<QArrayOfPathProperties> callsReply;
        QDBusPendingReply<QVariantMap> propsReply;
        QDBusPendingCallWatcher *calls_watcher, *props_watcher;

	// unsync, but feel relief about recursion in manager proxy.
#if 0
        // Force this to be sync to ensure we have initial properties
        watcher->waitForFinished();
        getCallsFinished(watcher);
        delete call_watcher;
#endif

        propsReply = GetProperties();
        props_watcher = new QDBusPendingCallWatcher(propsReply);

        connect(props_watcher,
                SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(getPropertiesFinished(QDBusPendingCallWatcher*)));

        callsReply = GetCalls();
        calls_watcher = new QDBusPendingCallWatcher(callsReply);

        connect(calls_watcher,
                SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(getCallsFinished(QDBusPendingCallWatcher*)));

        connect(this,
                SIGNAL(CallAdded(const QDBusObjectPath&, const QVariantMap&)),
                SLOT(callAdded(const QDBusObjectPath&, const QVariantMap&)));
        connect(this,
                SIGNAL(CallRemoved(const QDBusObjectPath&)),
                SLOT(callRemoved(const QDBusObjectPath&)));
        connect(this,
                SIGNAL(PropertyChanged(const QString&, const QDBusVariant&)),
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

QList<QString> CallManager::callsAsStrings() const
{
    TRACE
    return m_calls;
}

int CallManager::multipartyCallCount() const
{
    TRACE
    int call_count = 0;
    foreach (CallItem *c, m_callItems) {
        if(c->multiparty()) {
            call_count++;
        }
    }
    return call_count;
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

CallItem *CallManager::heldCall() const
{
    TRACE
    if (m_callItems.size())
    foreach (CallItem *c, m_callItems)
        if (c->state() == CallItemModel::STATE_HELD)
            return c;
    return NULL;
}

CallItem *CallManager::dialingCall() const
{
    TRACE
    if (m_callItems.size())
    foreach (CallItem *c, m_callItems)
        if (c->state() == CallItemModel::STATE_DIALING)
            return c;
    return NULL;
}

CallItem *CallManager::incomingCall() const
{
    TRACE
    if (m_callItems.size())
    foreach (CallItem *c, m_callItems)
        if (c->state() == CallItemModel::STATE_INCOMING)
            return c;
    return NULL;
}

CallItem *CallManager::waitingCall() const
{
    TRACE
    if (m_callItems.size())
    foreach (CallItem *c, m_callItems)
        if (c->state() == CallItemModel::STATE_WAITING)
            return c;
    return NULL;
}

CallItem *CallManager::alertingCall() const
{
    TRACE
    if (m_callItems.size())
    foreach (CallItem *c, m_callItems)
        if (c->state() == CallItemModel::STATE_ALERTING)
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
    ModemProxy* p = ManagerProxy::instance()->modem();

    // Nothing to do if the modem is not powered up
    if(!p->powered()) {
        emit callsChanged();
        return;
    }

    // If not online (flight mode?), check if the requested number is
    // one of the allowed EmergencyNumbers, in which case, continue.
    // Otherwise, notify that only Emergency calls are permitted.
    if(!p->online()) {
        if(p->powered() && !m_emergencyNumbers.contains(number)) {
            emit callsChanged();
            emit onlyEmergencyCalls();
            return;
        }
    }

    if (ResourceProxy::instance())
        ResourceProxy::instance()->acquireDialResource(number);
}

void CallManager::deniedCallDial()
{
    TRACE

    QString message = QString("Denied: Dial resource");
    qCritical() << message;

    emit callResourceLost(message);
}

void CallManager::lostCallDial()
{
    TRACE

    QString message = QString("Lost: Dial resource");
    qCritical() << message;

    hangupAll();
    emit callResourceLost(message);
}

void CallManager::proceedCallDial(const QString number)
{
    TRACE

    QDBusPendingReply<QDBusObjectPath> reply;
    QDBusPendingCallWatcher *watcher;

    reply = Dial(stripLineID(number), QString());
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(dialFinished(QDBusPendingCallWatcher*)));
}

void CallManager::deniedCallAnswer()
{
    TRACE

    QString message = QString("Denied: Call resource");
    qCritical() << message;

    hangupAll();

    emit callResourceLost(message);
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
 * Joins the currently Active (or Outgoing, depending
 * on network support) and Held calls together and
 * disconnects both calls. In effect transfering
 * one party to the other. This procedure requires
 * an Active and Held call and the Explicit Call Transfer
 * (ECT) supplementary service to be active.
 */
void CallManager::transferCalls()
{
    TRACE

    QDBusPendingReply<> reply;
    QDBusPendingCallWatcher *watcher;

    reply = Transfer();
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(transferFinished(QDBusPendingCallWatcher*)));
}

/*
 * Releases currently active call and answers the currently
 * waiting call. Please note that if the current call is
 * a multiparty call, then all parties in the multi-party
 * call will be released.
 */
void CallManager::releaseAndAnswer()
{
    TRACE

    QDBusPendingReply<> reply;
    QDBusPendingCallWatcher *watcher;

    reply = ReleaseAndAnswer();
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(releaseAndAnswerFinished(QDBusPendingCallWatcher*)));
}

/*
 * Places the multi-party call on hold and makes desired
 * call active. This is used to accomplish private chat
 * functionality.  Note that if there are only two calls
 * (three parties) in the multi-party call the result will
 * be two regular calls, one held and one active. The
 * Multiparty call will need to be setup again by using the
 * CreateMultiparty method.  Returns the new list of calls
 * participating in the multiparty call.
 */
void CallManager::privateChat(const CallItem &call)
{
    TRACE

    QDBusPendingReply<QList<QDBusObjectPath> > reply;
    QDBusPendingCallWatcher *watcher;

    reply = PrivateChat(QDBusObjectPath(call.path()));
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(privateChatFinished(QDBusPendingCallWatcher*)));
}

/*
 * Joins active and held calls together into a multi-party
 * call. If one of the calls is already a multi-party
 * call, then the other call is added to the multiparty
 * conversation. Returns the new list of calls
 * participating in the multiparty call.
 *
 * There can only be one subscriber controlled multi-party
 * call according to the GSM specification.
 */
void CallManager::createMultipartyCall()
{
    TRACE

    QDBusPendingReply<QList<QDBusObjectPath> > reply;
    QDBusPendingCallWatcher *watcher;

    reply = CreateMultiparty();
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(createMultipartyFinished(QDBusPendingCallWatcher*)));
}

/*
 * Hangs up the multi-party call.  All participating
 * calls are released.
 */
void CallManager::hangupMultipartyCall()
{
    TRACE

    QDBusPendingReply<> reply;
    QDBusPendingCallWatcher *watcher;

    reply = HangupMultiparty();
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(hangupMultipartyFinished(QDBusPendingCallWatcher*)));
}

void CallManager::sendTones(const QString toneid)
{
    TRACE

    QDBusPendingReply<> reply;
    QDBusPendingCallWatcher *watcher;

    reply = SendTones(toneid);
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     SLOT(sendTonesFinished(QDBusPendingCallWatcher*)));
}

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
        foreach (CallItem *item, m_callItems)
            delete item;
        m_callItems.clear();
        emit callsChanged();

        if (ResourceProxy::instance())
            ResourceProxy::instance()->releaseResources();

        return;
    }

    // Remove CallItems that are not in the ofono "calls" list
    QMutableListIterator<CallItem*> iter(m_callItems);
    while (iter.hasNext()) {
        CallItem *item = iter.next();
        // This item is not in the ofono list, remove it
        if (!m_calls.contains(item->path())) {
            qDebug() << QString("Removing old CallItem %1").arg(item->path());
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
            connect (call, SIGNAL(multipartyChanged()),SLOT(callMultipartyChanged()));
            m_callItems << call;

            // NOTE: Must explicity bubble this up since incoming and waiting
            //       calls do not "changeState" unless they are handled or
            //       timeout
            if (call->state() == CallItemModel::STATE_INCOMING) {
                if (ResourceProxy::instance())
                    ResourceProxy::instance()->acquireIncomingResource(call);
            } else if (call->state() == CallItemModel::STATE_WAITING) {
                if (ResourceProxy::instance())
                    ResourceProxy::instance()->acquireIncomingResource(call);
            } else {
                changed = true;
            }
        }
    }

    if (changed)
        emit callsChanged();
}

void CallManager::proceedIncomingCall(CallItem *call)
{
    TRACE

    qDebug() << QString("Acquired: Incoming Call resource");
    qDebug() << QString("Insert new CallItem %1").arg(call->path());

    emit incomingCall(call);
    emit callsChanged();
}

void CallManager::deniedIncomingCall(CallItem *call)
{
    TRACE

    qCritical() << QString("Denied: Incoming Call resource");
    qDebug() << QString("Insert new CallItem %1").arg(call->path());

    emit incomingCall(call);
    emit callsChanged();
}

void CallManager::lostIncomingCall(CallItem *call)
{
    TRACE
    Q_UNUSED(call)

    qCritical() << QString("Lost: Incoming Call resource");
}

void CallManager::setCalls(QList<QDBusObjectPath> calls)
{
    TRACE

    m_calls.clear();

    foreach (QDBusObjectPath c, calls)
        m_calls << QString(c.path());

    updateCallItems();
}

void CallManager::getPropertiesFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE

    ResourceProxy *resource = ResourceProxy::instance();
    QDBusPendingReply<QVariantMap> reply = *watcher;

    if (reply.isError()) {
        qCritical() << QString("GetProperties: Failed to connect to %1 on modem %2:\n\t%3")
                       .arg(staticInterfaceName())
                       .arg(path())
                       .arg(lastError().message());
        return;
    }

    QVariantMap props = reply.value();
    m_emergencyNumbers = qdbus_cast<QStringList>(props["EmergencyNumbers"].toStringList());
    qDebug() << QString("EmergencyNumbers = %1").arg(m_emergencyNumbers.join(","));

    // Indicate for this instance, that we've actually performed at least
    // one round trip call to this VoiceCallManager and we are in sync with it
    // First sucessfull GetProperties == connected
    if (!m_connected) {
        m_connected = true;
        emit connected();
        TRACE
    }

    // Resource proxy binding
    if (resource) {
        connect(resource, SIGNAL(incomingResourceAcquired(CallItem *)),
                SLOT(proceedIncomingCall(CallItem *)));
        connect(resource, SIGNAL(incomingResourceDenied(CallItem *)),
                SLOT(deniedIncomingCall(CallItem *)));
        connect(resource, SIGNAL(incomingResourceLost(CallItem *)),
                SLOT(lostIncomingCall(CallItem *)));

        connect(resource, SIGNAL(dialResourceAcquired(const QString)),
                SLOT(proceedCallDial(const QString)));
        connect(resource, SIGNAL(dialResourceDenied()),
                SLOT(deniedCallDial()));
        connect(resource, SIGNAL(callResourceLost()),
                SLOT(lostCallDial()));
    }
}

void CallManager::getCallsFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE

    QDBusPendingReply<QArrayOfPathProperties> reply = *watcher;

    if (reply.isError()) {
        qCritical() << QString("GetCalls: %1 request failed on modem %2:\n\t%3")
                       .arg(staticInterfaceName())
                       .arg(path())
                       .arg(lastError().message());
        return;
    }

    QArrayOfPathProperties props = reply.value();

    QList<QDBusObjectPath> calls, mpcalls;
    foreach (OfonoPathProperties p, props) {
        calls   << p.path;
    }

    setCalls(calls);
}

void CallManager::callAdded(const QDBusObjectPath &in0,const QVariantMap &in1)
{
    Q_UNUSED(in1)
    TRACE

    QString path = in0.path();

    qDebug() << QString("CallAdded: \"%1\"").arg(path);

    m_calls << path;

    updateCallItems();
}

void CallManager::callRemoved(const QDBusObjectPath &in0)
{
    TRACE

    QString path = in0.path();

    qDebug() << QString("CallRemoved: \"%1\"").arg(path);
    if (m_calls.contains(path))
        m_calls.removeAt(m_calls.indexOf(path));

    updateCallItems();
}

void CallManager::dialFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE

    QDBusPendingReply<QDBusObjectPath> reply = *watcher;

    if (reply.isError()) {
        qCritical() << QString("Dial() Failed: %1 - %2")
                       .arg(reply.error().name())
                       .arg(reply.error().message());
        // Fix BMC#10848:
        // Notify that state of the call has changed when the dialing fails
        emit callsChanged();
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

void CallManager::transferFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE
    QDBusPendingReply<> reply = *watcher;

    if (reply.isError())
        qCritical() << QString("Transfer() Failed: %1 - %2")
                       .arg(reply.error().name())
                       .arg(reply.error().message());
}

void CallManager::releaseAndAnswerFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE
    QDBusPendingReply<> reply = *watcher;

    if (reply.isError())
        qCritical() << QString("ReleaseAndAnswer() Failed: %1 - %2")
                       .arg(reply.error().name())
                       .arg(reply.error().message());
}

void CallManager::privateChatFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE

    QDBusPendingReply<QList<QDBusObjectPath> > reply = *watcher;

    if (reply.isError()) {
        qCritical() << QString("PrivateChat() Failed: %1 - %2")
                       .arg(reply.error().name())
                       .arg(reply.error().message());
        return;
    } else {
        QList<QDBusObjectPath> val = reply.value();
        qDebug() << QString("PrivateChat() Success: paths ==");
        if (val.size()) {
            foreach (QDBusObjectPath p, val) {
                qDebug() << QString("------> %1").arg(p.path());
            }
        }
    }
}

void CallManager::createMultipartyFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE

    QDBusPendingReply<QList<QDBusObjectPath> > reply = *watcher;

    if (reply.isError()) {
        qCritical() << QString("CreateMultiparty() Failed: %1 - %2")
                       .arg(reply.error().name())
                       .arg(reply.error().message());
        return;
    } else {
        QList<QDBusObjectPath> val = reply.value();
        qDebug() << QString("CreateMultiparty() Success: paths ==");
        if (val.size()) {
            foreach (QDBusObjectPath p, val) {
                qDebug() << QString("------> %1").arg(p.path());
            }
        }
    }
}

void CallManager::hangupMultipartyFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE

    QDBusPendingReply<> reply = *watcher;

    if (reply.isError())
        qCritical() << QString("HangupMultiparty() Failed: %1 - %2")
                       .arg(reply.error().name())
                       .arg(reply.error().message());
}

void CallManager::sendTonesFinished(QDBusPendingCallWatcher *watcher)
{
    TRACE
    QDBusPendingReply<> reply = *watcher;

    if (reply.isError())
        qCritical() << QString("SendTones() Failed: %1 - %2")
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
    } else if (in0 == "EmergencyNumbers") {
        m_emergencyNumbers = qdbus_cast<QStringList>(in1.variant());
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

void CallManager::callMultipartyChanged()
{
    TRACE
    emit callsChanged();
}

QStringList CallManager::dumpProperties()
{
    m_properties.clear();
    QStringList l_multipartyCalls;

    // Single party calls
    m_properties << "<ul><li>Calls:</li>";
    if (m_callItems.size())
        foreach (CallItem *c, m_callItems) {
            m_properties << QString("<ul><li>Path: %1</li>").arg(c->path());
            m_properties << QString("<li>LineID  : %1</li>").arg(c->lineID());
            m_properties << QString("<li>State   : %1</li>").arg(c->state());
            m_properties << QString("<li>Started : %1</li>")
                            .arg(c->startTime().toString());
            if (c->multiparty())
                l_multipartyCalls << c->path();

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
    if (l_multipartyCalls.size() >= 2)
        foreach (QString path, l_multipartyCalls) {
            m_properties << QString("<ul><li>Path: %1</li>").arg(path);
            m_properties << QString("</ul></ul>");
        }
    else
        m_properties << "<ul><li>None</li></ul></ul>";

    return m_properties;
}

void CallManager::error(const QString message)
{
    qCritical() << QString("Streamer error: %1").arg(message);
}
