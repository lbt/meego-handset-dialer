/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef ALERTDIALOG_H
#define ALERTDIALOG_H

#include "callitem.h"
#include <MDialog>
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MLinearLayoutPolicy>
#include <MStylableWidget>
#include <MImageWidget>
#include <MButton>
#include <MLabel>

class MLayout;
class MGridLayoutPolicy;

class AlertDialog : public MDialog
{
    Q_OBJECT

public:
    AlertDialog();
    virtual ~AlertDialog();

    void setCallItem(CallItem*);

private Q_SLOTS:
    void acceptCall();
    void declineCall();
    void callStateChanged();
    void hideKeypad();
    void showKeypad();

private:
    void init();
    void updateInfo();

    MStylableWidget   *m_panel;
    MLayout           *m_layout;
    MGridLayoutPolicy *m_land;
    MGridLayoutPolicy *m_port;
    MButton           *m_accept;
    MButton           *m_decline;
    MImageWidget      *m_photo;
    MLabel            *m_name;
    MLabel            *m_number;

    CallItem            *m_call;
};

#endif // ALERTDIALOG_H
