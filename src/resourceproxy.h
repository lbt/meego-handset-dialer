/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2010, 2011, Nokia Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef RESOURCEPROXY_H
#define RESOURCEPROXY_H

#include <policy/resource-set.h>
#include <QObject>

#include "callitem.h"

class ResourceProxy : public QObject
{
    Q_OBJECT

public:
    ~ResourceProxy(void);

    void acquireAnswerResource(void);
    void acquireDialResource(const QString);
    void acquireIncomingResource(CallItem *call);

    void releaseResources(void);

    static ResourceProxy *instance(void);

signals:
    void answerResourceAcquired(void);
    void answerResourceDenied(void);

    void dialResourceAcquired(const QString);
    void dialResourceDenied(void);

    void callResourceLost(void);

    void incomingResourceAcquired(CallItem *call);
    void incomingResourceDenied(CallItem *call);
    void incomingResourceLost(CallItem *call);

private slots:
    void acquireHandlerRingtone(void);
    void lostHandlerRingtone(void);
    void releaseHandlerRingtone(void);
    void deniedHandlerRingtone(void);

    void acquireHandlerCall(void);
    void lostHandlerCall(void);
    void releaseHandlerCall(void);
    void deniedHandlerCall(void);

protected:
    ResourceProxy(QObject *parent = 0);

private:
    ResourceProxy(const ResourceProxy&);
    ResourceProxy& operator= (const ResourceProxy&);

    ResourcePolicy::ResourceSet *resourceSetRingtone;
    ResourcePolicy::ResourceSet *resourceSetCall;

    enum ResourceState {
	Released,
	Acquired,
    } stateCall, stateRingtone;

    QString  dialedNumber;
    CallItem *incomingCall;

    static ResourceProxy *gResource;
};

#endif // RESOURCEPROXY_H
