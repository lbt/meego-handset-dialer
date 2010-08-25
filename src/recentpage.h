/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef RECENTPAGE_H
#define RECENTPAGE_H

#include "common.h"
#include "genericpage.h"
#include "historytablemodel.h"
#include <QSortFilterProxyModel>
#include <QTimer>
#include <MWidget>
#include <MTextEdit>
#include <MList>
#include <QModelIndex>
#include <MCompleter>
#include <MButton>

class RecentItemCellCreator;

class RecentPage : public GenericPage
{
    Q_OBJECT

public:

    RecentPage();
    virtual ~RecentPage();
    virtual void createContent();

public slots:
    void pageShown();

private:
    MTextEdit           *entry;
    MList               *events;
    QSortFilterProxyModel *filter;
    HistoryTableModel     *people;
    MCompleter *completer;
    RecentItemCellCreator *cellCreator;
    MButton             *bksp;
    MButton             *dial;
    bool                   pressed;
    QTimer                 tapnhold;

private slots:
    void matchSelected(const QModelIndex&);
    void startCompleting(const QString&);
    void checkForEmpty();
    void handleDialClick();
    void doClear();
    void doBackspace();
    void handleBkspPress();
    void handleBkspRelease();
};

#include <mabstractcellcreator.h>
#include <seasidelistitem.h>

class RecentItemCellCreator : public MAbstractCellCreator<SeasideListItem>
{
public:
    RecentItemCellCreator();
    void updateCell(const QModelIndex& index, MWidget * cell) const;
    QDateTime qDateTimeFromString(const QString&) const;
};

#endif // RECENTPAGE_H
