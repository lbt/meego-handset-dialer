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
#include "recentpage.h"
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
#include <QDebug>
#include <QAbstractListModel>
#include <seaside.h>
#include <MSceneManager>
#include <mwidgetanimation.h>
#include <QTextCursor>

#include <MWidgetCreator>
M_REGISTER_WIDGET(RecentPage)

void RecentPage::pageShown()
{
    TRACE
    startCompleting(entry->text());
}

void RecentPage::activateWidgets()
{
    TRACE
    // Add any setup necessary for display of this page
    // then call the genericpage setup
    // Add your code here

    GenericPage::activateWidgets();
}

void RecentPage::deactivateAndResetWidgets()
{
    TRACE
    // Add any cleanup code for display of this page
    // then call the generic page cleanup
    // Add your code here

    GenericPage::deactivateAndResetWidgets();
}

RecentPage::RecentPage() :
    GenericPage(),
    entry(new MTextEdit()),
    events(new MList()),
    filter(0),
    people(0),
    completer(new MCompleter()),
    cellCreator(new RecentItemCellCreator()),
    //% "<-"
    bksp(new MButton("icon-m-common-backspace",qtTrId("xx_backspace"))),
    //% "Call"
    dial(new MButton("icon-m-telephony-call",qtTrId("xx_call"))),
    pressed(false),
    tapnhold(this)
{
    TRACE
    m_pageType = GenericPage::PAGE_RECENT;
    setObjectName("RecentPage");
    filter = DA_HISTORYPROXY;
    people = DA_HISTORYMODEL;

    bksp->setTextVisible(false);
    dial->setTextVisible(false);

    // Disable panning, as it eliminates our ability to assure the buttons
    // or text edit are always visible (kinetic return does not return to
    // original position, just "close" to it)
    setPannable(false);
}

RecentPage::~RecentPage()
{
    TRACE
}

void RecentPage::matchSelected(const QModelIndex &index)
{
    TRACE
    QString result = index.model()->index(index.row(),HistoryTableModel::COLUMN_LINEID)
                         .data().value<QString>();
    if (result.isEmpty()) {
        result.clear();
        result=QString("");
    }

    // When an item is selected, invoke the "call"
    if (!result.isEmpty()) {
        qDebug() << QString("Calling selected contact number = ") << result;
        dynamic_cast<MainWindow *>(applicationWindow())->call(result);
    }
    entry->clear();
}

void RecentPage::startCompleting(const QString & prefix)
{
    TRACE
    QRegExp   exp = filter->filterRegExp();
    int filterCol = HistoryTableModel::COLUMN_LINEID;
    int   sortCol = HistoryTableModel::COLUMN_CALLSTART;
    int filterRole = HistoryTableModel::CompleterRole;
    Qt::SortOrder order = Qt::DescendingOrder;

    if (!completer->candidateSourceModel())
        completer->setCandidateSourceModel(filter);

    if (prefix.isEmpty())
        exp = MATCH_ALL;
    else
        exp = MATCH_ANY(stripLineID(prefix));

    filter->setFilterRole(filterRole);
    filter->setFilterKeyColumn(filterCol);
    filter->setFilterRegExp(exp);
    filter->sort(sortCol, order);
    // FIXME: Seems I need to do this to force the the widgets that use
    //        the QSortFilterProxyModel to get dataChanged or layoutChanged
    completer->model()->setMatchedModel(filter);
    events->setItemModel(filter);
}

void RecentPage::checkForEmpty()
{
    TRACE
    if (entry->text().isEmpty())
        startCompleting("");
}

void RecentPage::createContent()
{
    TRACE
    GenericPage::createContent();

    // Widget creation
    MContainer *box = new MContainer();
    box->setObjectName("listBox");
    box->setHeaderVisible(false);
    MPannableViewport *view = new MPannableViewport();
    view->setObjectName("listView");
    box->setCentralWidget(view);

    // Widget properties
    bksp->setObjectName("bkspButton");
    dial->setObjectName("dialButton");
    entry->setObjectName("phoneNumber");
    //% "Enter a number"
    entry->setPrompt(qtTrId("xx_number_prompt"));
    entry->setContentType(M::PhoneNumberContentType);
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

    landscape->addItem(box,    0, 0, 2, 1, Qt::AlignLeft);
    landscape->addItem(entry,  1, 0, 1, 1, Qt::AlignLeft);

    portrait->addItem(entry,   0, 0, 1, 3, Qt::AlignRight);
    portrait->addItem(bksp,    0, 3, 1, 1, Qt::AlignRight);
/* FIXME: Remove dial button from view for now, eventually should delete it
    portrait->addItem(dial,    0, 4, 1, 1, Qt::AlignLeft);
*/
    portrait->addItem(box,     1, 0, 1, 4, Qt::AlignCenter);

    // Match list creation
    events->setObjectName("matchList");
    events->setViewType("fast");
    events->setShowGroups(false);
    events->setSelectionMode(MList::SingleSelection);

    cellCreator->setCellObjectName("matchListItem");

    events->setCellCreator(cellCreator);
    events->setItemModel(filter);

    // Add it to the Pannable Viewport
    view->setWidget(events);

    entry->setCompleter(completer);

    // Force the filtering to reset to nothing if the text edit is empty
    connect(entry, SIGNAL(textChanged()), this, SLOT(checkForEmpty()));

    // Forcebly prevent the completer list from showing
    connect(completer, SIGNAL(shown()), completer, SLOT(hideCompleter()));

    // Impliment our own match method
    connect(completer, SIGNAL(startCompleting(const QString&)),
            this, SLOT(startCompleting(const QString&)));

    // Add the phone number of the selected item when a selection occurs
    connect(events, SIGNAL(itemClicked(const QModelIndex&)),
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

void RecentPage::handleDialClick()
{
    TRACE
    dynamic_cast<MainWindow *>(applicationWindow())->call(entry->text());
}

void RecentPage::doClear()
{
    TRACE
    pressed = false;
    entry->clear();
    startCompleting("");
}

void RecentPage::doBackspace()
{
    TRACE
    entry->textCursor().deletePreviousChar();
    startCompleting(entry->text());
}

void RecentPage::handleBkspPress()
{
    TRACE
    pressed = true;
    tapnhold.start(500);
    connect(&tapnhold, SIGNAL(timeout()), this, SLOT(doClear()));
}

void RecentPage::handleBkspRelease()
{
    TRACE
    if (tapnhold.isActive())
        tapnhold.stop();

    if (pressed) {
        pressed = false;
        doBackspace();
    }
}

/*
 * Cell Creator class implimentation
 */
RecentItemCellCreator::RecentItemCellCreator()
{
//    setCellObjectName("matchListItem");
}

void RecentItemCellCreator::updateCell(const QModelIndex& index,
                                       MWidget * cell) const
{
    TRACE

    if (!cell)
        return;

    Q_ASSERT(index.isValid());

    SeasideListItem *card = qobject_cast<SeasideListItem *>(cell);

    //% "Unknown Caller"
    QString name(qtTrId("xx_unknown"));
    QString photo("default-contact-photo");
    QString lineid = index.model()->index(index.row(),HistoryTableModel::COLUMN_LINEID).data().value<QString>();
    Seaside::Presence presence = Seaside::PresenceUnknown;
    bool favorite = false;
    QString uuid;
    int commType = Seaside::CommNone;

    SeasideSyncModel *contacts = DA_SEASIDEMODEL;
    QModelIndex first = contacts->index(0,Seaside::ColumnPhoneNumbers);
    int role = Seaside::SearchRole;
    int hits = -1;
    Qt::MatchFlags flags = (Qt::MatchContains|Qt::MatchWrap);
    QModelIndexList matches;
    matches.clear();

    if (!lineid.isEmpty()) {
        // FIXME: Need to strip the lineid of invalid phone chars for now
        //        until libseaside can properly match non-stripped values
        lineid = stripLineID(lineid);

        matches = contacts->match(first,role,QVariant(lineid),hits);
        if (matches.count()) {
            QModelIndex person = matches.at(0); //First match is all we look at
            SEASIDE_SHORTCUTS
            SEASIDE_SET_MODEL_AND_ROW(person.model(), person.row());

            QString firstName = SEASIDE_FIELD(FirstName, String);
            QString lastName = SEASIDE_FIELD(LastName, String);

            if (lastName.isEmpty())
                // Contacts first (common) name
                //% "%1"
                name = qtTrId("xx_first_name").arg(firstName);
            else
                // Contacts full, sortable name, is "Lastname, Firstname"
                //% "%1, %2"
                name = qtTrId("xx_full_name").arg(lastName).arg(firstName);

            uuid = SEASIDE_FIELD(Uuid, String);
            photo = SEASIDE_FIELD(Avatar, String);
            presence = (Seaside::Presence)SEASIDE_FIELD(Presence,Int);
            favorite = SEASIDE_FIELD(Favorite,Bool);
            commType = SEASIDE_FIELD(CommType,Int);
        } else {
            qDebug() << QString("No match found for \"%1\" in libseaside").arg(lineid);
        }
    } else {
        //% "Unavailable"
        lineid = qtTrId("xx_unavailable");
    }

    card->setUuid(uuid);
    card->setName(name);
    card->setThumbnail(photo);
    card->setPresence(presence);
    card->setFavorite(favorite);
    card->setDetails(QStringList(lineid));
    card->setButton("icon-m-telephony-call");

    // The rest comes from the history event, not the contact data
    QDateTime startTime = qDateTimeFromOfono(index.model()->
              index(index.row(),HistoryTableModel::COLUMN_CALLSTART)
              .data().value<QString>());
    // Use Epoch if the value stored in History model is bogus
    if (!startTime.isValid())
        startTime = QDateTime::fromTime_t(0);
    card->setStatus(startTime.toString());

    int dir = index.model()->index(index.row(),HistoryTableModel::COLUMN_DIRECTION).data().value<int>();
    if (dir == 0)
        card->setCommFlags(Seaside::CommCallDialed);
    else if (dir == 1)
        card->setCommFlags(Seaside::CommCallReceived);
    else if (dir == 2)
        card->setCommFlags(Seaside::CommCallMissed);
    else
        card->setCommFlags(Seaside::CommCallMissed);
}
