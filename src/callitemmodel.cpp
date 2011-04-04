/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "callitemmodel.h"

const QString CallItemModel::lineID() const
{
    if (call() && call()->isValid())
        return call()->lineID();
    else
        return QString();
}

const QString CallItemModel::name() const
{
    if (call() && call()->isValid())
        return call()->name();
    else
        return QString();
}

const QString CallItemModel::state() const
{
    if (call() && call()->isValid())
        return call()->state();
    else
        return QString();
}

const QString CallItemModel::stateTr() const
{
    QString l_stateTr;
    if (state() == "active")
        //% "active"
        l_stateTr = qtTrId("xx_state_active");
    else if (state() == "held")
        //% "held"
        l_stateTr = qtTrId("xx_state_held");
    else if (state() == "dialing")
        //% "dialing"
        l_stateTr = qtTrId("xx_state_dialing");
    else if (state() == "alerting")
        //% "alerting"
        l_stateTr = qtTrId("xx_state_alerting");
    else if (state() == "incoming")
        //% "incoming"
        l_stateTr = qtTrId("xx_state_incoming");
    else if (state() == "waiting")
        //% "waiting"
        l_stateTr = qtTrId("xx_state_waiting");
    else if (state() == "disconnected")
        //% "disconnected"
        l_stateTr = qtTrId("xx_state_disconnected");
    else
        //% "invalid"
        l_stateTr = qtTrId("xx_state_invalid");
    return l_stateTr;
}

CallItemModel::CallState CallItemModel::stateType() const
{
    CallState l_state = STATE_NONE;
    if (state() == "active")
        l_state = STATE_ACTIVE;
    else if (state() == "held")
        l_state = STATE_HELD;
    else if (state() == "dialing")
        l_state = STATE_DIALING;
    else if (state() == "alerting")
        l_state = STATE_ALERTING;
    else if (state() == "incoming")
        l_state = STATE_INCOMING;
    else if (state() == "waiting")
        l_state = STATE_WAITING;
    else if (state() == "disconnected")
        l_state = STATE_DISCONNECTED;
    return l_state;
}

const QDateTime CallItemModel::startTime() const
{
    if (call() && call()->isValid())
        return call()->startTime();
    else
        return QDateTime();
}

const QString CallItemModel::reason() const
{
    if (call() && call()->isValid())
        return call()->reason();
    else
        return QString();
}

CallItemModel::CallDisconnectReason CallItemModel::reasonType() const
{
    CallDisconnectReason l_reason = DISCONNECT_NONE;
    if (reason() == "local")
        l_reason = DISCONNECT_LOCAL;
    else if (reason() == "remote")
        l_reason = DISCONNECT_REMOTE;
    else if (reason() == "network")
        l_reason = DISCONNECT_NETWORK;

    return l_reason;
}

int CallItemModel::duration() const
{
    if (call() && call()->isValid())
        return call()->duration();
    else
        return 0;
}

bool CallItemModel::active() const
{
    return (stateType() == STATE_ACTIVE);
}

bool CallItemModel::held() const
{
    return (stateType() == STATE_HELD);
}

bool CallItemModel::dialing() const
{
    return (stateType() == STATE_DIALING);
}

bool CallItemModel::alerting() const
{
    return (stateType() == STATE_ALERTING);
}

bool CallItemModel::incoming() const
{
    return (stateType() == STATE_INCOMING);
}

bool CallItemModel::waiting() const
{
    return (stateType() == STATE_WAITING);
}

bool CallItemModel::disconnected() const
{
    return (stateType() == STATE_DISCONNECTED);
}
