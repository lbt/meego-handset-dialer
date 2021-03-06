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
#include "dialerapplication.h"
#include "peopleitem.h"
#include "peoplepage.h"
#include "managerproxy.h"
#include "mainwindow.h"
#include "dialerkeypad.h"
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MTextEdit>
#include <MLabel>
#include <MImageWidget>
#include <MContentItem>
#include <MList>
#include <MPannableViewport>
#include <MCompleter>
#include <QDebug>
#include <QStringListModel>
#include <QAbstractListModel>
#include <seaside.h>
#include <MSceneManager>
#include <mwidgetanimation.h>
#include <QTextCursor>

#include <MWidgetCreator>
M_REGISTER_WIDGET(PeoplePage)

void PeoplePage::pageShown()
{
    TRACE
    startCompleting(entry->text());
}

void PeoplePage::activateWidgets()
{
    TRACE
    // Add any setup necessary for display of this page
    // then call the genericpage setup
    // Add your code here

    GenericPage::activateWidgets();
}

void PeoplePage::deactivateAndResetWidgets()
{
    TRACE
    // Add any cleanup code for display of this page
    // then call the generic page cleanup
    // Add your code here

    GenericPage::deactivateAndResetWidgets();
}

PeoplePage::PeoplePage() :
    GenericPage(), entry(new MTextEdit()),
    matches(new MList()), incall(false),
    filter(0), completer(new MCompleter()),
    people(0),
    cellCreator(new PeopleItemCellCreator()),
    //% "<-"
    bksp(new MButton("icon-m-common-backspace",qtTrId("xx_backspace"))),
    //% "Call"
    dial(new MButton("icon-m-telephony-call",qtTrId("xx_call"))),
    pressed(false),
    tapnhold(this)
{
    TRACE
    m_pageType = GenericPage::PAGE_PEOPLE;
    setObjectName("PeoplePage");
    filter = DA_SEASIDEPROXY;
    people = DA_SEASIDEMODEL;

    bksp->setTextVisible(false);
    dial->setTextVisible(false);

    // Disable panning, as it eliminates our ability to assure the buttons
    // or text edit are always visible (kinetic return does not return to
    // original position, just "close" to it)
    setPannable(false);
}

PeoplePage::~PeoplePage()
{
    TRACE
}

void PeoplePage::matchSelected(const QModelIndex &index)
{
    TRACE
    int row = index.row();
    QStringList result = index.model()->index(row,Seaside::ColumnPhoneNumbers)
                         .data(Seaside::DataRole).value<QStringList>();
    if (result.isEmpty()) {
        result.clear();
        result << QString("");
    }

    // When an item is selected, invoke the "call"
    if (!result.at(0).isEmpty()) {
        qDebug() << QString("Calling selected contact number = ") << result.at(0);
        dynamic_cast<MainWindow *>(applicationWindow())->call(result.at(0));
    }
    entry->clear();
}

void PeoplePage::startCompleting(const QString & prefix)
{
    TRACE
    QRegExp   exp = filter->filterRegExp();
    int filterCol = filter->filterKeyColumn();
    int   sortCol = filter->sortColumn();
    int  sortRole = Seaside::DataRole;
    SeasideProxyModel::FilterType seasideFilter = SeasideProxyModel::FilterAll;
    Qt::SortOrder order = Qt::AscendingOrder;

    if (!completer->candidateSourceModel())
        completer->setCandidateSourceModel(filter);

    sortCol = Seaside::ColumnLastName;

    if (prefix.isEmpty())
        exp = MATCH_ALL;
    else
        exp = MATCH_ANY(prefix);

    filter->setFilterRole(Seaside::SearchRole);
    filter->setSortRole(sortRole);
    filter->setFilter(seasideFilter);
    filter->setFilterRegExp(exp);
    filter->setFilterKeyColumn(filterCol);
    filter->sort(sortCol, order);
    // FIXME: Seems I need to do this to force the the widgets that use
    //        the QSortFilterProxyModel to get dataChanged or layoutChanged
    completer->model()->setMatchedModel(filter);
    matches->setItemModel(filter);
}

void PeoplePage::checkForEmpty()
{
    TRACE
    if (entry->text().isEmpty())
        startCompleting("");
}

void PeoplePage::createContent()
{
    TRACE
    GenericPage::createContent();

    // Widget creation
    MContainer *box = new MContainer();
    box->setObjectName("listBox");
    MPannableViewport *view = new MPannableViewport();
    view->setObjectName("listView");
    box->setCentralWidget(view);
    box->setHeaderVisible(false);

    // Widget properties
    bksp->setObjectName("bkspButton");
    dial->setObjectName("dialButton");
    entry->setObjectName("phoneNumber");
    //entry->setPrompt("Enter a number");
    //entry->setContentType(M::PhoneNumberContentType);
    //% "Type name to filter"
    entry->setPrompt(qtTrId("xx_filter_prompt"));
    entry->setContentType(M::FreeTextContentType);
    /* FIXME:
       This hack is to prevent the nav bar from hiding on focus.  The default
       behavior of the MTextEdit is to invoke the SIP from the scene, which
       in turn, causes the MNavigationBar to hide.  This is not configurable,
       and worse, when focus is lost, the restored nav bar, will now contain
       the escape button, even if it was "hidden" by the MApplicationPage

       NOTE: setFocusPolicy *MUST* be called after setTextInteractionFlags in
       order for the widget to not take focus at all.  Setting TextEditable is
       required inorder for us to call insert() on it when the keypad buttons
       are pressed.
     */
    entry->setTextInteractionFlags(Qt::TextEditorInteraction);
    //entry->setFocusPolicy(Qt::NoFocus);

    landscape->addItem(box,    0, 0, 2, 1, Qt::AlignLeft);
    landscape->addItem(entry,  1, 0, 1, 1, Qt::AlignLeft);

    portrait->addItem(entry,   0, 0, 1, 3, Qt::AlignRight);
    portrait->addItem(bksp,    0, 3, 1, 1, Qt::AlignRight);
/* FIXME: Remove dial button from view for now, eventually should delete it
    portrait->addItem(dial,    0, 4, 1, 1, Qt::AlignLeft);
*/
    portrait->addItem(box,     1, 0, 1, 4, Qt::AlignCenter);

    // Match list creation
    matches->setObjectName("matchList");
    matches->setViewType("fast");
    matches->setShowGroups(false);
    matches->setSelectionMode(MList::SingleSelection);

    cellCreator->setCellObjectName("matchListItem");

    matches->setCellCreator(cellCreator);
    matches->setItemModel(filter);

    // Add it to the Pannable Viewport
    view->setWidget(matches);

    entry->setCompleter(completer);

    // Force the filtering to reset to nothing if the text edit is empty
    connect(entry, SIGNAL(textChanged()), this, SLOT(checkForEmpty()));

    // Forcebly prevent the completer list from showing
    connect(completer, SIGNAL(shown()), completer, SLOT(hideCompleter()));

    // Impliment our own match method
    connect(completer, SIGNAL(startCompleting(const QString&)),
            this, SLOT(startCompleting(const QString&)));

    // Add the phone number of the selected item when a selection occurs
    connect(matches, SIGNAL(itemClicked(const QModelIndex&)),
            this, SLOT(matchSelected(const QModelIndex&)));

    // Set the Keypad Target when this window is shown
    connect(this, SIGNAL(appeared()), SLOT(pageShown()));

    // Hook up dial button
    connect(dial, SIGNAL(clicked()), SLOT(handleDialClick()));

    // Hook up backspace key
    tapnhold.setSingleShot(true);
    connect(bksp, SIGNAL(pressed()), SLOT(handleBkspPress()));
    connect(bksp, SIGNAL(released()), SLOT(handleBkspRelease()));
}

void PeoplePage::handleDialClick()
{
    TRACE
    dynamic_cast<MainWindow *>(applicationWindow())->call(entry->text());
}

void PeoplePage::doClear()
{
    TRACE
    pressed = false;
    entry->clear();
    startCompleting("");
}

void PeoplePage::doBackspace()
{
    TRACE
    entry->textCursor().deletePreviousChar();
    startCompleting(entry->text());
}

void PeoplePage::handleBkspPress()
{
    TRACE
    pressed = true;
    tapnhold.start(500);
    connect(&tapnhold, SIGNAL(timeout()), this, SLOT(doClear()));
}

void PeoplePage::handleBkspRelease()
{
    TRACE
    if (tapnhold.isActive())
        tapnhold.stop();

    if (pressed) {
        pressed = false;
        doBackspace();
    }
}
