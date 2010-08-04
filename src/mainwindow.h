/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MAINWINDOW_h
#define MAINWINDOW_h

#include "genericpage.h"
#include "callitem.h"
#include "alertdialog.h"
#include "dialerkeypad.h"
#include "searchbar.h"
#include <QDebug>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MRemoteAction>
#include <MMessageBox>

class MainWindow: public MApplicationWindow
{
    Q_OBJECT

public:
    MainWindow();
    void showDebugPage();
    void simulateIncomingCall();
    bool event(QEvent *event);
    SearchBar *searchBar();
#ifdef IVI_HFP
    void displayBannerMessage(QString msg);
#endif
    DialerKeyPad *keypad();

    QList<GenericPage *> m_pages;

public Q_SLOTS:
    void handleIncomingCall(CallItem *call);
    void call(QString no);
    void accept();
    void showTBD();

private Q_SLOTS:
    void callStateChanged();

private:
    MApplicationPage *m_lastPage;
    AlertDialog        *m_alert;
    SearchBar          *m_search;
    DialerKeyPad       *m_keypad;
    MRemoteAction       m_acceptAction;
    CallItem           *m_incomingCall;
    MMessageBox        *m_tbd;

    Q_DISABLE_COPY(MainWindow);
};

#endif // MAINWINDOW_H
