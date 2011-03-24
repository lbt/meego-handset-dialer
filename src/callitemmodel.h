/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef CALLITEMMODEL_H
#define CALLITEMMODEL_H

#include "callproxy.h"
#include <QString>
#include <QDateTime>
#include <mwidgetmodel.h>

class M_EXPORT CallItemModel: public MWidgetModel
{
    Q_OBJECT
    M_MODEL(CallItemModel)

public:
    enum CallState {
        STATE_NONE = 0,
        STATE_ACTIVE,
        STATE_HELD,
        STATE_DIALING,
        STATE_ALERTING,
        STATE_INCOMING,
        STATE_WAITING,
        STATE_DISCONNECTED,
        STATE_LAST,
    };

    enum CallDirection {
        DIRECTION_NONE = 0,
        DIRECTION_IN,
        DIRECTION_OUT,
        DIRECTION_MISSED,
        DIRECTION_LAST,
    };

    enum CallDisconnectReason {
        DISCONNECT_NONE = 0,
        DISCONNECT_LOCAL,
        DISCONNECT_REMOTE,
        DISCONNECT_NETWORK,
        DISCONNECT_LAST,
    };

    M_MODEL_PTR_PROPERTY(CallProxy*, call, Call, true, NULL)
    M_MODEL_PROPERTY(CallItemModel::CallDirection, direction, Direction, true , DIRECTION_NONE)
    M_MODEL_PROPERTY(bool, selected, Selected, true, false)

public:
    const QString lineID() const;
    const QString name() const;
    const QString state() const;
    const QString stateTr() const;
    const QDateTime startTime() const;
    const QString reason() const;
    int  duration() const;
    bool active() const;
    bool held() const;
    bool dialing() const;
    bool alerting() const;
    bool incoming() const;
    bool waiting() const;
    bool disconnected() const;
    CallItemModel::CallState stateType() const;
    CallItemModel::CallDisconnectReason reasonType() const;
};

#endif // CALLITEMMODEL_H
