/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "modemproxy.h"
#include <QDebug>

ModemProxy::ModemProxy(const QString &objectPath)
    : org::ofono::Modem(OFONO_SERVICE,
                        objectPath,
                        QDBusConnection::systemBus()),
      m_interfaces(0)
{
    if (!isValid()) {
        qDebug() << "Failed to connect to Ofono: \n\t" << lastError().message();
    } else {
        m_path = objectPath;
        QDBusPendingReply<QVariantMap> reply;
        QDBusPendingCallWatcher * watcher;

        reply = GetProperties();
        watcher = new QDBusPendingCallWatcher(reply);

        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                this, SLOT(modemDBusGetPropDone(QDBusPendingCallWatcher*)));
    }
}

ModemProxy::~ModemProxy()
{
}

QStringList ModemProxy::interfaces() const { return m_interfaces; }
QString ModemProxy::path() const { return m_path; }
QString ModemProxy::manufacturer() const { return m_manufacturer; }
QString ModemProxy::model() const { return m_model; }
QString ModemProxy::revision() const { return m_revision; }
QString ModemProxy::serial() const { return m_serial; }
bool    ModemProxy::powered() const { return m_powered; }

void ModemProxy::setPowered(bool is_powered)
{
    if (m_powered == is_powered)
        return;

    QDBusPendingReply<QVariantMap> reply;
    reply = SetProperty("Powered", QDBusVariant(m_powered?"true":"false"));
    if (reply.isError())
        qCritical() << "SetProperty \"Powered\" failed!";
    else
        m_powered = is_powered;
}

void ModemProxy::modemDBusGetPropDone(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QVariantMap> reply = *call;

    if (reply.isError()) {
      // TODO: Handle this properly, by setting states, or disabling features...
      qDebug() << "org.ofono.ModemProxy.getProperties() failed: " <<
                  reply.error().message();
    } else {
      QVariantMap properties = reply.value();
      m_interfaces = qdbus_cast<QStringList >(properties["Interfaces"]);
      m_manufacturer = qdbus_cast<QString>(properties["Manufacturer"]);
      m_model = qdbus_cast<QString>(properties["Model"]);
      m_powered = qdbus_cast<bool>(properties["Powered"]);
      m_revision = qdbus_cast<QString>(properties["Revision"]);
      m_serial = qdbus_cast<QString>(properties["Serial"]);

#ifdef WANT_DEBUG
      qDebug() << "Modem Details:";
      qDebug() << dumpProperties().join("");
#endif

      // First sucessfull GetProperties == connected
      if (!m_connected) {
          m_connected = true;
          emit connected();
      }
    }
}

QStringList ModemProxy::dumpProperties()
{
    m_properties.clear();

    m_properties << "<ul>";
    m_properties << "<li>Manufacturer: " + m_manufacturer + "</li>";
    m_properties << "<li>Model: " + m_model + "</li>";
    m_properties << "<li>Version: " + m_revision + "</li>";
    m_properties << "<li>Serial: " + m_serial + "</li>";
    m_properties << "<li>Power: " + QString(m_powered?"On":"Off") + "</li>";
    m_properties << "<li>Services: </li><ul>";
    foreach (QString s, m_interfaces) m_properties << "<li>" + s + "</li>";
    m_properties << "</ul></ul>";

    return m_properties;
}

/*
 * CallVolume Manager implementation
 */

VolumeManager::VolumeManager(const QString &objectPath)
    : org::ofono::CallVolume(OFONO_SERVICE,
                             objectPath,
                             QDBusConnection::systemBus())
{
    if (!isValid()) {
        qDebug() << "Failed to connect to Ofono: \n\t" << lastError().message();
    } else {
        m_path = objectPath;
        QDBusPendingReply<QVariantMap> reply;
        QDBusPendingCallWatcher * watcher;

        reply = GetProperties();
        watcher = new QDBusPendingCallWatcher(reply);

        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                this, SLOT(volumeDBusGetPropDone(QDBusPendingCallWatcher*)));
    }
}

VolumeManager::~VolumeManager()
{
}

QString VolumeManager::path() const { return m_path; }
int     VolumeManager::speakerVolume() const { return m_speakerVolume; }
int     VolumeManager::micVolume() const { return m_micVolume; }
bool    VolumeManager::muted() const { return m_muted; }

void VolumeManager::setSpeakerVolume(int volume)
{
    if (m_speakerVolume == volume)
        return;

    if ((volume < 0) || (volume > 100)) {
        qWarning() << "SpeakerVolume value out of range (0<>100)";
        return;
    }

    QDBusPendingReply<> reply;
    reply = SetProperty("SpeakerVolume", QDBusVariant(volume));
    reply.waitForFinished();

    if (reply.isError())
        qCritical() << "SetProperty \"SpeakerVolume\" failed: " <<
                       reply.error().message();
    else
        m_speakerVolume = volume;
}

void VolumeManager::setMicVolume(int volume)
{
    if (m_micVolume == volume)
        return;

    if ((volume < 0) || (volume > 100)) {
        qWarning() << "MicrophoneVolume value out of range (0<>100)";
        return;
    }

    QDBusPendingReply<> reply;
    reply = SetProperty("MicrophoneVolume", QDBusVariant(volume));
    reply.waitForFinished();

    if (reply.isError())
        qCritical() << "SetProperty \"MicrophoneVolume\" failed: " <<
                       reply.error().message();
    else
        m_micVolume = volume;
}

void VolumeManager::setMuted(bool is_muted)
{
    if (m_muted == is_muted)
        return;

    QDBusPendingReply<> reply;
    reply = SetProperty("Muted", QDBusVariant(is_muted));
    reply.waitForFinished();

    if (reply.isError())
        qCritical() << "SetProperty \"Muted\" failed: " <<
                       reply.error().message();
    else
        m_muted = is_muted;
}

void VolumeManager::volumeDBusGetPropDone(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QVariantMap> reply = *call;

    if (reply.isError()) {
      // TODO: Handle this properly, by setting states, or disabling features...
      qDebug() << "org.ofono.CallVolume.getProperties() failed: " <<
                  reply.error().message();
    } else {
      QVariantMap properties = reply.value();
      m_speakerVolume = qdbus_cast<int>(properties["SpeakerVolume"]);
      m_micVolume = qdbus_cast<int>(properties["MicrophoneVolume"]);
      m_muted = qdbus_cast<bool>(properties["Muted"]);

#ifdef WANT_DEBUG
      qDebug() << "Volume Details:";
      qDebug() << dumpProperties().join("");
#endif

      // First sucessfull GetProperties == connected
      if (!m_connected) {
          m_connected = true;
          emit connected();
      }
    }
}

QStringList VolumeManager::dumpProperties()
{
    m_properties.clear();

    m_properties << "<ul>";
    m_properties << "<li>SpeakerVolume: " +
                    QString::number(m_speakerVolume) + "</li>";
    m_properties << "<li>MicrophoneVolume: " +
                    QString::number(m_micVolume) + "</li>";
    m_properties << "<li>Muted: " + QString(m_muted?"true":"false") + "</li>";
    m_properties << "</ul>";

    return m_properties;
}
