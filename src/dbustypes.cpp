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

// Marshall the CallHistoryEvent data into a D-BUS argument
QDBusArgument & operator << (QDBusArgument &argument,
                             const CallHistoryEvent &d)
{
    argument.beginStructure();
    argument << d.id << d.lineid << d.type << d.start << d.end;
    argument.endStructure();
    return argument;
}

// Retrieve the CallHistoryEvent data from the D-BUS argument
const QDBusArgument & operator >> (const QDBusArgument &argument,
                                   CallHistoryEvent &d)
{
    argument.beginStructure();
    argument >> d.id >> d.lineid >> d.type >> d.start >> d.end;
    argument.endStructure();
    return argument;
}
