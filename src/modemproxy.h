/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MODEMPROXY_H
#define MODEMPROXY_H

#include "modem_interface.h"
#include <QtDBus>
#include <QDebug>

#define OFONO_SERVICE "org.ofono"
#define OFONO_MANAGER_PATH "/"

class ModemProxy: public org::ofono::Modem
{
    Q_OBJECT
    Q_PROPERTY(QStringList interfaces READ interfaces)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(QString manufacturer READ manufacturer)
    Q_PROPERTY(QString model READ model)
    Q_PROPERTY(QString revision READ revision)
    Q_PROPERTY(QString serial READ serial)
    Q_PROPERTY(bool powered READ powered WRITE setPowered)
    Q_PROPERTY(bool online READ online WRITE setOnline)

public:
    ModemProxy(const QString &objectPath);
    virtual ~ModemProxy();

    QStringList interfaces() const;
    QString path() const;
    QString manufacturer() const;
    QString model() const;
    QString revision() const;
    QString serial() const;
    bool powered() const;
    bool online() const;

    QStringList dumpProperties();

public slots:
    void setPowered(bool is_powered);
    void setOnline(bool is_online);

    void modemDBusGetPropDone(QDBusPendingCallWatcher *call);

Q_SIGNALS:
    void poweredChanged(bool powered);
    void onlineChanged(bool powered);
    void connected();
    void disconnected();

private:
    QStringList m_properties; // raw return from GetProperties
    QStringList m_interfaces;
    QString     m_path;
    QString     m_manufacturer;
    QString     m_model;
    QString     m_revision;
    QString     m_serial;
    bool        m_powered;
    bool        m_online;
    bool        m_connected;
};

class VoicemailProxy: public org::ofono::MessageWaiting
{
    Q_OBJECT
    Q_PROPERTY(bool waiting READ waiting)
    Q_PROPERTY(int count  READ count)
    Q_PROPERTY(QString mailbox READ mailbox WRITE setMailbox)

public:
    VoicemailProxy(const QString &objectPath);
    virtual ~VoicemailProxy();

    QString  path() const;
    bool     waiting() const;
    int      count() const;
    QString  mailbox() const;

    QStringList dumpProperties();

public slots:
    void setMailbox(QString lineid);

Q_SIGNALS:
    void messagesWaitingChanged();
    void mailboxChanged();
    void connected();
    void disconnected();

private slots:
    void voicemailDBusGetPropDone(QDBusPendingCallWatcher *call);
    void voicemailPropertyChanged(const QString &in0, const QDBusVariant &in1);

private:
    QStringList m_properties; // raw return from GetProperties
    QString     m_path;
    bool        m_waiting;
    int         m_count;
    QString     m_mailbox;
    bool        m_connected;
};

class VolumeManager: public org::ofono::CallVolume
{
    Q_OBJECT
    Q_PROPERTY(int speakerVolume READ speakerVolume WRITE setSpeakerVolume)
    Q_PROPERTY(int micVolume READ micVolume WRITE setMicVolume)
    Q_PROPERTY(bool muted READ muted WRITE setMuted)

public:
    VolumeManager(const QString &objectPath);
    virtual ~VolumeManager();

    QString path() const;
    int  speakerVolume() const;
    int  micVolume() const;
    bool muted() const;

    QStringList dumpProperties();

public slots:
    void setSpeakerVolume(int volume);
    void setMicVolume(int volume);
    void setMuted(bool is_muted);
    void volumeDBusGetPropDone(QDBusPendingCallWatcher *call);

Q_SIGNALS:
    void speakerVolumeChanged(int volume);
    void micVolumeChanged(int volume);
    void muteChanged(bool muted);
    void connected();
    void disconnected();

private:
    QStringList m_properties; // raw return from GetProperties
    QString     m_path;
    int         m_speakerVolume;
    int         m_micVolume;
    bool        m_muted;
    bool        m_connected;
};

#endif
