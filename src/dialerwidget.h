/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef DIALERWIDGET_H
#define DIALERWIDGET_H

#include <MWidgetController>
#include <QList>

class MButton;
class MLabel;
class MGridLayoutPolicy;

enum labels { num_0 = 0, num_9 = 9 };

enum DialerViewType {
    DialerViewTypeInfo      = 0,
    DialerViewTypeDebug     = 1,
    DialerViewTypeInCall    = 2,
    DialerViewTypePeople    = 3,
};

typedef enum _DialerStateType {
    DialerStateNominal     = (1<<0),
    DialerStateIncoming    = (1<<1),
    DialerStateConnected   = (1<<2),
    DialerStateDialer      = (1<<3),
    DialerStatePeople      = (1<<4),
} DialerStateType;

typedef enum _PeopleStateType {
    PeopleStateAll       = (1<<0),
    PeopleStateRecent    = (1<<1),
    PeopleStateFavorites = (1<<2),
    PeopleStateSearch    = (1<<3),
} PeopleStateType;

enum CallOptionHoldState {
    CallHoldStateUnheld = 0,
    CallHoldStateHeld   = 1,
};

enum CallOptionMuteState {
    CallMuteStateUnmuted = 0,
    CallMuteStateMuted   = 1,
};

enum CallOptionSpeakerState {
    CallSpeakerStateHandset = 0,
    CallSpeakerStateSpeaker = 1,
};


class DialerWidget : public MWidget
{
public:
    DialerWidget();

private:
    MLabel * mView;
    MLabel * mKeypad;
    MLabel * mEntry;
};

#endif // DIALERWIDGET_H
