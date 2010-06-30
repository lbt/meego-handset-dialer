/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include "common.h"
#include <QObject>
#include <MOverlay>
#include <MTextEdit>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MStylableWidget>

class SearchBar : public MOverlay
{
    Q_OBJECT;

public:
    SearchBar(MWidget *parent = 0);
    virtual ~SearchBar();

    SearchBar   *instance();
    MTextEdit *entry();

private:
    MStylableWidget     *m_box;
    MLayout             *m_layout;
    MLinearLayoutPolicy *m_policy;
    MTextEdit           *m_entry;
};

#endif // SEARCHBAR_H
