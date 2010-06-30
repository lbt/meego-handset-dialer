/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef DIALERPAGE_H
#define DIALERPAGE_H

#include "genericpage.h"
#include "callitem.h"
#include <MTextEdit>
#include <MLinearLayoutPolicy>
#include <MButton>

class DialerPage : public GenericPage
{
    Q_OBJECT

public:

    DialerPage();
    virtual ~DialerPage();
    virtual void createContent();

private Q_SLOTS:
    void pageShown();
    void pageHidden();
    void updateCalls();
    void doClear();
    void doBackspace();
    void handleBkspPress();
    void handleBkspRelease();

private:
    CallItem    *m_activeCall;
    CallItem    *m_inactiveCall;
    MTextEdit   *m_entry;
    bool         m_incall;
    MLayout     *m_layout;
    MLinearLayoutPolicy *m_policy;
    MButton     *m_bksp;
    bool         m_pressed;
    QTimer       m_tapnhold;
};

#endif // DIALERPAGE_H
