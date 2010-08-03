/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef CALLITEMSTYLE_H
#define CALLITEMSTYLE_H

#include <mwidgetstyle.h>

class CallItemStyle : public MWidgetStyle
{
    Q_OBJECT

    M_STYLE (CallItemStyle)

    M_STYLE_ATTRIBUTE (QString, photoImage, PhotoImage)
    M_STYLE_ATTRIBUTE (QString, peopleItemObjectName, PeopleItemObjectName)
    M_STYLE_ATTRIBUTE (QString, statusObjectName, StatusObjectName)
    M_STYLE_ATTRIBUTE (QString, durationObjectName, DurationObjectName)
};

class CallItemStyleContainer : public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(CallItemStyle)

    M_STYLE_MODE(Active)
    M_STYLE_MODE(Held)
    M_STYLE_MODE(Dialing)
    M_STYLE_MODE(Alerting)
    M_STYLE_MODE(Incoming)
    M_STYLE_MODE(Waiting)
    M_STYLE_MODE(Disconnected)
};

#endif // CALLITEMSTYLE_H
