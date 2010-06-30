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
#include "favoritespage.h"
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
M_REGISTER_WIDGET(FavoritesPage)

void FavoritesPage::pageShown()
{
    TRACE
    startCompleting(entry->text());
}

FavoritesPage::FavoritesPage() :
    GenericPage(), entry(new MTextEdit()),
    matches(new MList()), incall(false),
    filter(0), completer(new MCompleter()),
    people(0),
    cellCreator(new PeopleItemCellCreator()),
    bksp(new MButton("icon-m-common-backspace","<[x]")),
    dial(new MButton("icon-m-telephony-call","Call")),
    pressed(false),
    tapnhold(this)
{
    TRACE
    m_pageType = GenericPage::PAGE_FAVORITE;
    setObjectName("FavoritesPage");
    filter = DA_SEASIDEPROXY;
    people = DA_SEASIDEMODEL;

    bksp->setTextVisible(false);
    dial->setTextVisible(false);

    // Disable panning, as it eliminates our ability to assure the buttons
    // or text edit are always visible (kinetic return does not return to
    // original position, just "close" to it)
    setPannable(false);
}

FavoritesPage::~FavoritesPage()
{
    TRACE
}

void FavoritesPage::matchSelected(const QModelIndex &index)
{
    TRACE
    int row = index.row();
    QStringList result = index.model()->index(row,Seaside::ColumnPhoneNumbers)
                         .data(Seaside::DataRole).value<QStringList>();
    if (result.isEmpty()) {
        result.clear();
        result << QString("");
    }
    entry->setText(result.at(0));
    qDebug() << QString("Selected contact number = ") << result.at(0);
    startCompleting(entry->text());
}

void FavoritesPage::startCompleting(const QString & prefix)
{
    TRACE
    QRegExp   exp;
    int filterCol = filter->filterKeyColumn();
    int   sortCol = Seaside::ColumnName;
    int  sortRole = Seaside::DataRole;
    SeasideProxyModel::FilterType seasideFilter = SeasideProxyModel::FilterFavorites;
    Qt::SortOrder order = Qt::AscendingOrder;

    if (!completer->candidateSourceModel())
        completer->setCandidateSourceModel(filter);

    if (prefix.isEmpty())
        exp = MATCH_ALL;
    else
        exp = MATCH_ANY(prefix);

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

void FavoritesPage::checkForEmpty()
{
    TRACE
    if (entry->text().isEmpty())
        startCompleting("");
}

void FavoritesPage::createContent()
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
    entry->setPrompt("Type name to filter");
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
    entry->setTextInteractionFlags(Qt::TextEditable);
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

    filter->setSortRole(Seaside::DataRole);
    filter->setFilter(SeasideProxyModel::FilterFavorites);
    filter->sort(Seaside::ColumnName, Qt::AscendingOrder);

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

void FavoritesPage::handleDialClick()
{
    TRACE
    dynamic_cast<MainWindow *>(applicationWindow())->call(entry->text());
}

void FavoritesPage::doClear()
{
    TRACE
    pressed = false;
    entry->clear();
    startCompleting("");
}

void FavoritesPage::doBackspace()
{
    TRACE
    entry->textCursor().deletePreviousChar();
    startCompleting(entry->text());
}

void FavoritesPage::handleBkspPress()
{
    TRACE
    pressed = true;
    tapnhold.start(500);
    connect(&tapnhold, SIGNAL(timeout()), this, SLOT(doClear()));
}

void FavoritesPage::handleBkspRelease()
{
    TRACE
    if (tapnhold.isActive())
        tapnhold.stop();

    if (pressed) {
        pressed = false;
        doBackspace();
    }
}
