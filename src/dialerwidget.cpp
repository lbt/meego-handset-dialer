/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "dialerwidget.h"
#include "dialerkeypad.h"
#include <QtGui>
#include <MLabel>
#include <MButton>
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MSceneManager>

DialerWidget::DialerWidget()
{
    QStateMachine *smach = new QStateMachine(this);
    QState *state1 = new QState();
    QState *state2 = new QState();

    smach->addState(state1);
    smach->addState(state2);

    /* Create a MLayout attached to this widget */
    MLayout *layout = new MLayout(this);

    MGridLayoutPolicy *landscapePolicy = new MGridLayoutPolicy(layout);
    MGridLayoutPolicy *portraitPolicy = new MGridLayoutPolicy(layout);
    layout->setPortraitPolicy(portraitPolicy);
    layout->setLandscapePolicy(landscapePolicy);

    landscapePolicy->setObjectName( "dialerLandscape" );
    portraitPolicy->setObjectName( "dialerPortrait" );

    // Widget creation
    mEntry = new MLabel("Enter a number");
    mEntry->setObjectName( "entryLine" );
            mEntry->setAlignment( Qt::AlignRight );

    DialerKeyPad *keypad = new DialerKeyPad();
    MButton *button = new MButton ("Change State");

    landscapePolicy->addItem( button, 0, 0);
    landscapePolicy->addItem( mEntry, 0, 1);
    landscapePolicy->addItem( keypad, 0, 2);

    portraitPolicy->addItem( button, 0, 0);
    portraitPolicy->addItem( mEntry, 1, 0);
    portraitPolicy->addItem( keypad, 2, 0);
    \
    state1->addTransition(button, SIGNAL(clicked()), state2);
    state2->addTransition(button, SIGNAL(clicked()), state1);

    // Assign widget properties to states
    state2->assignProperty(button, "text", "state 2");
    state2->assignProperty(keypad, "visible", false);

    state1->assignProperty(button, "text", "state 1");
    state1->assignProperty(keypad, "visible", true);

    smach->setInitialState(state1);
    smach->start();
}
