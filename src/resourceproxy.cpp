/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2010, 2011, Nokia Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QDebug>

#include "managerproxy.h"
#include "resourceproxy.h"

using namespace ResourcePolicy;

static ResourceProxy *gResource = 0;

void ResourceProxy::acquireAnswerResource(void)
{
    qDebug("ResourceProxy::%s state == %d %d", __func__, stateCall, stateRingtone);

    dialedNumber.clear();

    if (stateCall == Released)
        resourceSetCall->acquire();
    else
        emit answerResourceAcquired();
}

void ResourceProxy::acquireDialResource(const QString number)
{
    qDebug("ResourceProxy::%s state == %d %d", __func__, stateCall, stateRingtone);

    dialedNumber = number;

    if (stateCall == Released)
	resourceSetCall->acquire();
    else
	emit dialResourceAcquired(number);
}

void ResourceProxy::acquireIncomingResource(CallItem *call)
{
    qDebug("ResourceProxy::%s state == %d %d", __func__, stateCall, stateRingtone);

    if (stateRingtone == Released) {
        incomingCall = call;
	resourceSetRingtone->acquire();
    } else
	emit incomingResourceAcquired(call);
}

void ResourceProxy::releaseResources(void)
{
    qDebug("ResourceProxy::%s state == %d %d", __func__, stateCall, stateRingtone);

    if (stateRingtone == Acquired)
	resourceSetRingtone->release();

    if (stateCall == Acquired)
	resourceSetCall->release();
}

// Private slots connected to Resource library signals
void ResourceProxy::acquireHandlerRingtone(void)
{
    qDebug("ResourceProxy::%s state == %d %d", __func__, stateCall, stateRingtone);

    stateRingtone = Acquired;

    if (incomingCall)
        emit incomingResourceAcquired(incomingCall);
}

void ResourceProxy::lostHandlerRingtone(void)
{
    qDebug("ResourceProxy::%s state == %d %d", __func__, stateCall, stateRingtone);

    stateRingtone = Released;

    if (incomingCall)
	emit incomingResourceLost(incomingCall);

    incomingCall = NULL;
}

void ResourceProxy::deniedHandlerRingtone(void)
{
    qDebug("ResourceProxy::%s state == %d %d", __func__, stateCall, stateRingtone);

    stateRingtone = Released;

    if (incomingCall)
	emit incomingResourceDenied(incomingCall);

    incomingCall = NULL;
}

void ResourceProxy::releaseHandlerRingtone(void)
{
    qDebug("ResourceProxy::%s state == %d %d", __func__, stateCall, stateRingtone);

    stateRingtone = Released;

    incomingCall = NULL;
}

void ResourceProxy::acquireHandlerCall(void)
{
    qDebug("ResourceProxy::%s state == %d %d", __func__, stateCall, stateRingtone);

    stateCall = Acquired;

    if (dialedNumber.isEmpty())
        emit answerResourceAcquired();
    else
        emit dialResourceAcquired(dialedNumber);
}

void ResourceProxy::lostHandlerCall(void)
{
    qDebug("ResourceProxy::%s state == %d %d", __func__, stateCall, stateRingtone);

    stateCall = Released;

    emit callResourceLost();
}

void ResourceProxy::deniedHandlerCall(void)
{
    qDebug("ResourceProxy::%s state == %d %d", __func__, stateCall, stateRingtone);

    stateCall = Released;

    if (dialedNumber.isEmpty())
        emit answerResourceDenied();
    else
	emit dialResourceDenied();
}

void ResourceProxy::releaseHandlerCall(void)
{
    qDebug("ResourceProxy::%s state == %d %d", __func__, stateCall, stateRingtone);

    stateCall = Released;
}

ResourceProxy::ResourceProxy(QObject *parent) : QObject(parent)
{
    ResourcePolicy::AudioResource *audioResourceRingtone;
    ResourcePolicy::AudioResource *audioResourceCall;

    qDebug("ResourceProxy::%s", __func__);

    if (gResource)
	qFatal("ResourceProxy: There can be only one!");

    // Call resource set
    resourceSetCall = new ResourceSet("call", this);
    resourceSetCall->setAutoRelease();
    resourceSetCall->setAlwaysReply();

    audioResourceCall = new ResourcePolicy::AudioResource("call");
    audioResourceCall->setProcessID(QCoreApplication::applicationPid());
    audioResourceCall->setStreamTag("media.name", "*");

    resourceSetCall->addResourceObject(audioResourceCall);

    connect(resourceSetCall,
            SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType>&)), this,
            SLOT(acquireHandlerCall()));
    connect(resourceSetCall,
            SIGNAL(lostResources()), this,
            SLOT(lostHandlerCall()));
    connect(resourceSetCall,
            SIGNAL(resourcesReleased()), this,
            SLOT(releaseHandlerCall()));
    connect(resourceSetCall,
            SIGNAL(resourcesDenied()), this,
            SLOT(deniedHandlerCall()));

    // Ringtone resource set
    resourceSetRingtone = new ResourceSet("ringtone", this);
    resourceSetRingtone->setAutoRelease();
    resourceSetRingtone->setAlwaysReply();

    audioResourceRingtone = new ResourcePolicy::AudioResource("ringtone");
    audioResourceRingtone->setProcessID(QCoreApplication::applicationPid());
    audioResourceRingtone->setStreamTag("media.name", "*");

    resourceSetRingtone->addResourceObject(audioResourceRingtone);

    connect(resourceSetRingtone,
	    SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType>&)), this,
	    SLOT(acquireHandlerRingtone()));
    connect(resourceSetRingtone,
	    SIGNAL(lostResources()), this,
	    SLOT(lostHandlerRingtone()));
    connect(resourceSetRingtone,
	    SIGNAL(resourcesReleased()), this,
	    SLOT(releaseHandlerRingtone()));
    connect(resourceSetRingtone,
	    SIGNAL(resourcesDenied()), this,
	    SLOT(deniedHandlerRingtone()));

    // Resource proxy internals
    stateCall     = Released;
    stateRingtone = Released;

    dialedNumber.clear();
    incomingCall = NULL;

    gResource = this;
}

ResourceProxy::~ResourceProxy(void)
{
    qDebug("ResourceProxy::%s", __func__);

    if (gResource) {
        resourceSetRingtone->release();
        resourceSetCall->release();
        delete resourceSetCall;
        resourceSetCall = 0;
        delete resourceSetRingtone;
        resourceSetRingtone = 0;
    }

    dialedNumber.clear();
    incomingCall = NULL;

    gResource = 0;
}

ResourceProxy* ResourceProxy::instance(void)
{
    qDebug("ResourceProxy::%s", __func__);

    if (!gResource)
	gResource = new ResourceProxy();

    return gResource;
}
