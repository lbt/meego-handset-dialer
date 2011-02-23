/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef GENERICPAGE_H
#define GENERICPAGE_H

#include <MApplicationPage>

class MainWindow;
class MLayout;
class MGridLayoutPolicy;

class GenericPage : public MApplicationPage
{
    Q_OBJECT

public:

    enum PageType {
        PAGE_NONE     = -1,
        PAGE_DIALER   =  0,
        PAGE_RECENT   =  1,
        PAGE_PEOPLE   =  2,
        PAGE_FAVORITE =  3,
        PAGE_DEBUG    =  4,
    };

    GenericPage();
    virtual ~GenericPage();
    virtual void createContent();
    virtual MGridLayoutPolicy * policy(M::Orientation);

    MainWindow *mainWindow();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void activateWidgets();
    virtual void deactivateAndResetWidgets();

    MLayout *           layout;
    MGridLayoutPolicy * landscape;
    MGridLayoutPolicy * portrait;
    PageType            m_pageType;
};

#endif // GENERICPAGE_H
