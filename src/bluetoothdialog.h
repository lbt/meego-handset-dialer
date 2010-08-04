/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef BLUETOOTHDIALOG_H
#define BLUETOOTHDIALOG_H

#include "callitem.h"
#include <MDialog>
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MLinearLayoutPolicy>
#include <MStylableWidget>
#include <MImageWidget>
#include <MButton>
#include <MLabel>
#include <MDialog>

class MLayout;
class MGridLayoutPolicy;

class BluetoothDialog : public MDialog
{
    Q_OBJECT

public:
    BluetoothDialog();
    virtual ~BluetoothDialog();
    QString getSelectedModem();

Q_SIGNALS:
    void modemChanged(QString path);

private Q_SLOTS:
    void toggleBluetoothClicked();
    void cancelClicked();
    void openConnectDialog();
    void hideKeypad();
    void showKeypad();
    void modemSelected(QString path);

private:
    void init();
    void updateInfo();

    MStylableWidget   *m_panel;
    MLayout           *m_layout;
    MGridLayoutPolicy *m_land;
    MGridLayoutPolicy *m_port;
    MButton           *m_toggleBT;
    MButton           *m_cancel;
    MDialog*          m_modemListDialog;
};

#endif // BLUETOOTHDIALOG_H
