/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef NETWORKPROXY_H
#define NETWORKPROXY_H

#include "modem_interface.h"
#include "operator_interface.h"
#include <QtDBus>
#include <QDebug>

#define OFONO_SERVICE "org.ofono"
#define OFONO_MANAGER_PATH "/"

/* **************************************************************
 * Network Operator Class
 * **************************************************************/
class OperatorProxy: public org::ofono::NetworkOperator
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(QString countryCode READ countryCode)
    Q_PROPERTY(QString networkCode READ networkCode)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString status READ status)
    Q_PROPERTY(QStringList technologies READ technologies)

public:
    OperatorProxy(const QString &objectPath);
    virtual ~OperatorProxy();

    QString path() const;
    QString countryCode() const;
    QString networkCode() const;
    QString name() const;
    QString status() const;
    QStringList technologies() const;

    QStringList dumpProperties();

public slots:
    void operatorDBusGetPropDone(QDBusPendingCallWatcher *call);

Q_SIGNALS:
    void propertyChanged();

private:
    QStringList m_properties; // raw return from GetProperties
    QString     m_path;
    QString     m_countryCode;
    QString     m_networkCode;
    QString     m_name;
    QString     m_status;
    QStringList m_technologies;
};

/* **************************************************************
 * Network Registration Class
 * **************************************************************/
class NetworkProxy: public org::ofono::NetworkRegistration
{
    Q_OBJECT
    Q_PROPERTY(QList<OperatorProxy*> operators READ operators)
    Q_PROPERTY(OperatorProxy currentOperator READ currentOperator)
    Q_PROPERTY(QString mode READ mode)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString status READ status)

public:
    NetworkProxy(const QString &objectPath);
    virtual ~NetworkProxy();

    QList<OperatorProxy*> operators() const;
    OperatorProxy* currentOperator() const;
    QString mode() const;
    QString name() const;
    QString status() const;

    QStringList dumpProperties();

public slots:
    void networkDBusGetPropDone(QDBusPendingCallWatcher *call);

Q_SIGNALS:
    void propertyChanged();
    void connected();
    void disconnected();

private:
    OperatorProxy *m_currentOperator;

    QStringList m_properties; // raw return from GetProperties
    QStringList m_operatorPaths;
    QList<OperatorProxy*> m_operators;
    QString     m_mode;
    QString     m_name;
    QString     m_status;
    bool        m_connected;
};

#endif
