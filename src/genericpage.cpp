/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "common.h"
#include "genericpage.h"
#include "dialerapplication.h"
#include "mainwindow.h"
#include <QDebug>
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MLinearLayoutPolicy>
#include <MApplicationWindow>

#include <MWidgetCreator>
M_REGISTER_WIDGET(GenericPage)

GenericPage::GenericPage() :
    MApplicationPage(),
    layout(new MLayout(centralWidget())),
    landscape(new MGridLayoutPolicy(layout)),
    portrait(new MGridLayoutPolicy(layout)),
    m_pageType(PAGE_NONE)
{
    TRACE
    layout->setLandscapePolicy(landscape);
    layout->setPortraitPolicy(portrait);

    setEscapeMode(MApplicationPageModel::EscapeCloseWindow);
}

GenericPage::~GenericPage()
{
    TRACE
    delete layout;
    layout = 0;
}

//
// Reimpliments the showEvent to ensure the Header button states match
// the currently showing page
//
void GenericPage::showEvent(QShowEvent *event)
{
    TRACE
    Q_UNUSED(event)

    if (m_pageType == PAGE_NONE)
        return;

    MButton *pageButton=0;
    MButton *currentButton=0;
    TRACE
    pageButton = MainWindow::instance()->headerButtonGroup()->button(m_pageType);
    TRACE
    currentButton = MainWindow::instance()->headerButtonGroup()->checkedButton();
    TRACE

    if (pageButton && currentButton &&
        (pageButton != currentButton) && !pageButton->isChecked())
        pageButton->setChecked(true);
    TRACE
}

void GenericPage::createContent()
{
    TRACE
    MApplicationPage::createContent();
}

void GenericPage::activateWidgets()
{
    TRACE
    // Add any common page setup that needs to be done
    // to allow correct display when running in prestart mode
}

void GenericPage::deactivateAndResetWidgets()
{
    TRACE
    // Add any common page tear down that needs to be done
    // to allow correct display when running in prestart mode
}

MGridLayoutPolicy * GenericPage::policy(
                      M::Orientation orientation=M::Landscape)
{
    TRACE
    if (orientation == M::Portrait) {
        return portrait;
    } else {
        return landscape;
    }
}
MainWindow *GenericPage::mainWindow()
{
    TRACE
    return dynamic_cast<MainWindow *>(applicationWindow());
}
