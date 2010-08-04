/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MODEMLISTDIALOG_H
#define MODEMLISTDIALOG_H

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

class ModemListDialog : public MDialog
{
    Q_OBJECT

public:
    ModemListDialog();
    virtual ~ModemListDialog();

Q_SIGNALS:
    void modemSelected(QString path);

private Q_SLOTS:
    void cancelClicked();
    void connectModemClicked();

private:
    void init();
    void updateInfo();

    MStylableWidget   *m_panel;
    MLayout           *m_layout;
    MGridLayoutPolicy *m_land;
    MGridLayoutPolicy *m_port;
    MButton           *m_cancel;

    QHash<MButton *, QString> buttons;
};

#endif // MODEMLISTDIALOG_H
