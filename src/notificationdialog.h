/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

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

class NotificationDialog : public MDialog
{
    Q_OBJECT

public:
    NotificationDialog();
    virtual ~NotificationDialog();

    void setNotification(const QString);

private Q_SLOTS:
    void okPressed();
    void cleanupOnDismiss();
    void hideKeypad();
    void showKeypad();

private:
    void init();

    MStylableWidget   *m_panel;
    MLayout           *m_layout;
    MGridLayoutPolicy *m_land;
    MGridLayoutPolicy *m_port;
    MButton           *m_ok;
    MLabel            *m_message;
};

#endif // NOTIFICATIONDIALOG_H
