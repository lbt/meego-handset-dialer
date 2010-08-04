/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "dbustypes.h"
#include "managerproxy.h"
#include "manager_interface.h"
#include "dialerapplication.h"
#include <MNotification>
#include <QDebug>

static ManagerProxy *gManager = 0;

ManagerProxy::ManagerProxy(const QString &service,
                           const QString &path,
                           const QDBusConnection &connection,
                           QObject *parent)
    : org::ofono::Manager(service, path, connection, parent),
      m_modemPath (""),
      m_modem(0),
      m_network(0),
      m_callManager(0),
      m_volumeManager(0),
      m_history(0),
      m_voicemail(0)
{
    if (gManager)
        qFatal("ManagerProxy: There can be only one!");

    registerMyDataTypes();

    if (!isValid()) {
        qDebug() << "Failed to connect to Ofono: \n\t" << lastError().message();
    } else {
        QDBusPendingReply<QVariantMap> reply;
        QDBusPendingCallWatcher * watcher;

        reply = GetProperties();
        watcher = new QDBusPendingCallWatcher(reply);

        // Force this to be sync to ensure we have initial properties
        watcher->waitForFinished();
        managerDBusGetPropDone(watcher);
    }

    gManager = this;
}

ManagerProxy::~ManagerProxy()
{
    if (m_history)
        delete m_history;
    m_history = 0;

    if (m_volumeManager)
        delete m_volumeManager;
    m_volumeManager = 0;

    if (m_voicemail)
        delete m_voicemail;
    m_voicemail = 0;

    if (m_callManager)
        delete m_callManager;
    m_callManager = 0;

    if (m_network)
        delete m_network;
    m_network = 0;

    if (m_modem)
        delete m_modem;
    m_modem = 0;

    gManager=0;
}

void ManagerProxy::managerDBusGetPropDone(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QVariantMap> reply = *call;

    if (reply.isError()) {
      // TODO: Handle this properly, by setting states, or disabling features...
      qWarning() << "org.ofono.Manager.getProperties() failed: " <<
                    reply.error().message();
    } else {
      QVariantMap properties = reply.value();
      QList<QDBusObjectPath> paths =
          qdbus_cast<QList<QDBusObjectPath> >(properties["Modems"]);

      // Read the list of available Modems
      foreach (QDBusObjectPath p, paths) {
        // FIXME: Handle multiple modems...
        m_modemList.append(QString(p.path()));
        if (m_modemPath.isNull() || m_modemPath.isEmpty()) {
            m_modemPath = QString(p.path());
            m_modem = new ModemProxy(m_modemPath);
            m_network = new NetworkProxy(m_modemPath);
            m_callManager = new CallManager(m_modemPath);
            m_volumeManager = new VolumeManager(m_modemPath);
            m_voicemail = new VoicemailProxy(m_modemPath);
        // TODO: Connect to service proxies as available/needed here
        }
      }

      m_history = HistoryProxy::instance();
    }
}

ManagerProxy *ManagerProxy::instance()
{
    if (!gManager)
        gManager = new ManagerProxy();

    return gManager;
}

ModemProxy* ManagerProxy::modem() const
{
    return m_modem;
}

NetworkProxy* ManagerProxy::network() const
{
    return m_network;
}

CallManager* ManagerProxy::callManager() const
{
    return m_callManager;
}

VolumeManager* ManagerProxy::volumeManager() const
{
    return m_volumeManager;
}

HistoryProxy* ManagerProxy::history() const
{
    return m_history;
}

VoicemailProxy* ManagerProxy::voicemail() const
{
    return m_voicemail;
}

/*
 * Voice Call History ofono plugin manager class implimentation
 */

static HistoryProxy *gHistory = 0;

HistoryProxy::HistoryProxy(const QString &service,
                           const QString &path,
                           const QDBusConnection &connection,
                           QObject *parent)
    : org::ofono::CallHistory(service, path, connection, parent),
      m_cache(0)
{
    TRACE
    if (gHistory)
        qFatal("HistoryProxy: There can be only one!");

    if (!isValid()) {
        qWarning() << "HistoryProxy: Failed to connect to Ofono: \n\t"
                   << lastError().message();
    } else {
        initCache();

        connect(this, SIGNAL(VoiceHistoryChanged(uint)),
                this, SLOT(voiceHistoryChanged(uint)));
    }
    gHistory = this;
}

HistoryProxy::~HistoryProxy()
{
    TRACE
    m_cache->sync();

    gHistory=0;
}

HistoryProxy *HistoryProxy::instance()
{
    TRACE
    if (!gHistory)
        gHistory = new HistoryProxy();
    return gHistory;
}

void HistoryProxy::sendMissedCallNotification(QList<CallHistoryEvent> missed)
{
    TRACE
    foreach (CallHistoryEvent e, missed) {
        QString name;
        QString photo  = DEFAULT_AVATAR_ICON;
        QString lineid = "Private";
        QString summary("Missed call");
        QString body;
        MNotification notice(NOTIFICATION_CALL_EVENT);

        if (!e.lineid.isEmpty()) {
            lineid = stripLineID(e.lineid);
            SeasideSyncModel *contacts = DA_SEASIDEMODEL;
            QModelIndex first = contacts->index(0,Seaside::ColumnPhoneNumbers);
            QModelIndexList matches = contacts->match(first, Qt::DisplayRole,
                                                      QVariant(lineid),1);
            if (!matches.isEmpty()) {
                QModelIndex person = matches.at(0); //First match wins
                SEASIDE_SHORTCUTS
                SEASIDE_SET_MODEL_AND_ROW(person.model(), person.row());
                name = QString("%1, %2").arg(SEASIDE_FIELD(LastName, String))
                                        .arg(SEASIDE_FIELD(FirstName, String));
                photo = SEASIDE_FIELD(Avatar, String);
            }
        } else {
            lineid = "Unavailable";
        }

        body = QString("You missed a call from %1")
                              .arg(name.isEmpty()?lineid:name);

        notice.setSummary(summary);
        notice.setBody(body);
        notice.setImage(photo);
        notice.publish();

        qDebug() << QString("%1: %2").arg(summary).arg(body);
    }
}

void HistoryProxy::getHistoryFinished(QDBusPendingCallWatcher *call)
{
    TRACE
    QDBusPendingReply<QArrayOfHistoryEvent> reply = *call;

    if (reply.isError()) {
        qWarning() << QString("HistoryProxy: GetVoiceHistory() failed: %1")
                             .arg(reply.error().message());
    } else {
        QArrayOfHistoryEvent events = reply.value();

        if (!events.isEmpty()) {
            QList<CallHistoryEvent> missedCalls;

            m_cache->beginGroup("CallHistory");

            // Cache the new events
            QStringList ids;
            foreach (CallHistoryEvent e, events) {
                ids << QString::number(e.start);
                // Data fields from ofono are:
                //     e.id     (unit)   GUID for the call
                //     e.lineid (char*)  Phone number
                //     e.type   (ushort) Event type (missed, in, out, etc...)
                //     e.start  (int)    start time (as string)
                //     e.end    (int)    end time (as string)
                m_cache->beginGroup(QString::number(e.start));
                m_cache->setValue("LineID", e.lineid);
                m_cache->setValue("Type",   QString::number(e.type));
                m_cache->setValue("Start",  QString::number(e.start));
                m_cache->setValue("End",    QString::number(e.end));
                m_cache->endGroup(); // e.id;

                // Send notifications of "Missed" (type == 2) calls
                if (e.type == 2) {
                    missedCalls << e;
                }
            }
            m_cache->endGroup(); // "CallHistory"

            if (!missedCalls.isEmpty())
                sendMissedCallNotification(missedCalls);

            m_cache->sync();

            // Now ACK back to ofono that we've cached the latest events
            QDBusPendingReply<> ack = SetVoiceHistoryRead();

            // Notify that history has changed
            // FIXME:  I really need to properly hook up the model directly
            //         to this signal rather than forcing it from here.
            //emit historyChanged(ids);
            DialerApplication::instance()->historyModel()->appendRows(ids);
        }
    }
}

void HistoryProxy::voiceHistoryChanged(uint count)
{
    TRACE
    Q_UNUSED(count);
    QDBusPendingReply<QArrayOfHistoryEvent> reply;
    QDBusPendingCallWatcher * watcher;

    reply = GetVoiceHistory();
    watcher = new QDBusPendingCallWatcher(reply);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this,    SLOT(getHistoryFinished(QDBusPendingCallWatcher*)));
}

void HistoryProxy::initCache()
{
    TRACE
    // Initialize the on disk event cache (possibly poor use of QSettings?)
    m_cache = new QSettings("com.meego");
    if (m_cache->status() != QSettings::NoError) {
        if (m_cache->status() == QSettings::AccessError)
            qWarning("HistoryProxy: AccessError while initializing QSettings");
        else
            qWarning("HistoryProxy: FormatError while initializing QSettings");
    }

    // Manually call the VoiceHistoryChanged signal to get any pending
    // events that we may have missed since last instantiation
    voiceHistoryChanged(0);
}

QSettings *HistoryProxy::cache() const
{
    TRACE
    if (!m_cache) return NULL;

    return (m_cache->status() == QSettings::NoError)?m_cache:NULL;
}

QList<QString> ManagerProxy::getModemList()
{
    return m_modemList;
}

void ManagerProxy::setModem(QString modemPath)
{
    if (m_modemList.contains(modemPath)) {
        if (m_modem)
            delete m_modem;

        m_modem = new ModemProxy(modemPath);
    }
}

void ManagerProxy::setNetwork(QString modempath)
{
    if(m_modem->isValid()) {
        if (m_network)
            delete m_network;
        m_network = new NetworkProxy(modempath);
    }
}

void ManagerProxy::setCallManager(QString modempath)
{
    if(m_modem->isValid()) {
        if (m_callManager)
            delete m_callManager;
        m_callManager = new CallManager(modempath);
    }
}

