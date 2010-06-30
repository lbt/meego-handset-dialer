/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef DIALERDBUSTYPES_H
#define DIALERDBUSTYPES_H

#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtDBus/QtDBus>

struct CallHistoryEvent
{
    uint    id;
    QString lineid;
    ushort  type;
    int     start;
    int     end;
};

Q_DECLARE_METATYPE ( CallHistoryEvent )

// Marshall the CallHistoryEvent data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument,
                          const CallHistoryEvent &mystruct);

// Retrieve the CallHistoryEvent data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument,
                                CallHistoryEvent &mystruct);

typedef QList< CallHistoryEvent > QArrayOfHistoryEvent;

Q_DECLARE_METATYPE ( QArrayOfHistoryEvent )

inline void registerMyDataTypes() {
    qDBusRegisterMetaType< CallHistoryEvent >();
    qDBusRegisterMetaType< QArrayOfHistoryEvent >();
}

#endif   //DIALERDBUSTYPES_H
