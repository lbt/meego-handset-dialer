/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PEOPLEPAGE_H
#define PEOPLEPAGE_H

#include "genericpage.h"
#include <seasidesyncmodel.h>
#include <MButton>
#include <MTextEdit>
#include <MCompleter>
#include <MList>
#include <MButtonGroup>
#include <QModelIndex>
#include <QTimer>
#include <seasideproxymodel.h>
#include <MStylableWidget>
#include <MOverlay>
#include <MContainer>
#include <MButton>

class PeopleItemCellCreator;

class PeoplePage : public GenericPage
{
    Q_OBJECT

public:
    PeoplePage();
    virtual ~PeoplePage();
    virtual void createContent();

public slots:
    void pageShown();

private:
    MTextEdit * entry;
    MList   * matches;
    bool        incall;
    SeasideProxyModel *filter;
    MCompleter *completer;
    SeasideSyncModel *people;
    PeopleItemCellCreator *cellCreator;
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

#endif // PEOPLEPAGE_H
