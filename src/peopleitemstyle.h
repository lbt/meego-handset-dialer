/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PEOPLEITEMSTYLE_H
#define PEOPLEITEMSTYLE_H

#include <mwidgetstyle.h>

class PeopleItemStyle : public MWidgetStyle
{
    Q_OBJECT

    M_STYLE (PeopleItemStyle)

    M_STYLE_ATTRIBUTE (QString, nameObjectName, NameObjectName)
    M_STYLE_ATTRIBUTE (QString, phoneObjectName, PhoneObjectName)
    M_STYLE_ATTRIBUTE (QString, photoObjectName, PhotoObjectName)
    M_STYLE_ATTRIBUTE (QString, lastCommObjectName, LastCommObjectName)
    M_STYLE_ATTRIBUTE (QString, lastCommIconObjectName, LastCommIconObjectName)
    M_STYLE_ATTRIBUTE (QString, presenceObjectName, PresenceObjectName)
    M_STYLE_ATTRIBUTE (QString, favoriteIconObjectName, FavoriteIconObjectName)
};

class PeopleItemStyleContainer : public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(PeopleItemStyle)
};

#endif // PEOPLEITEMSTYLE_H
