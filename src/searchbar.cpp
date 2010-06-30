/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "searchbar.h"
#include <QDebug>
#include <MWidgetView>

#include <MWidgetCreator>
M_REGISTER_WIDGET(SearchBar)

static SearchBar *g_self = 0;

SearchBar::SearchBar(MWidget *parent)
    : MOverlay(parent),
      m_box(new MStylableWidget()),
      m_layout(new MLayout(m_box)),
      m_policy(new MLinearLayoutPolicy(m_layout, Qt::Horizontal)),
      m_entry(new MTextEdit())
{
    TRACE
    if (g_self)
        qFatal("SearchBar: There can be only one!");

    m_box->setParent(this);

    m_layout->setPolicy(m_policy);

    m_box->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                     QSizePolicy::MinimumExpanding));

    m_policy->insertItem(0, m_entry, Qt::AlignCenter);

    setObjectName("SearchBarOverlay");
    m_box->setObjectName("SearchBarBox");
    m_policy->setObjectName("SearchBarLayout");
    m_entry->setObjectName("SearchBarEntry");

    m_entry->setPrompt("Enter text to begin searching...");
    m_entry->setContentType(M::FreeTextContentType);
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
    m_entry->setTextInteractionFlags(Qt::TextEditable);
    //m_entry->setFocusPolicy(Qt::NoFocus);

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                              QSizePolicy::MinimumExpanding));

    setWidget(m_box);

    g_self = this;
}

SearchBar::~SearchBar()
{
    TRACE
    delete m_layout;
}

SearchBar *SearchBar::instance()
{
    TRACE
    if (!g_self)
        g_self = new SearchBar();
    return g_self;
}

MTextEdit *SearchBar::entry()
{
    TRACE
    return m_entry;
}
