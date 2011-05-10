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
#include "notificationdialog.h"
#include "dialerkeypad.h"
#include <QDebug>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MRemoteAction>
#include <MMessageBox>

class MainWindow: public MApplicationWindow
{
    Q_OBJECT

public:
    static MainWindow* instance();

    bool event(QEvent *event);

    DialerKeyPad *keypad();
    MButtonGroup *headerButtonGroup();

    QList<GenericPage *> m_pages;

public Q_SLOTS:
    void switchPage(int id);
    void switchPageNow(int id);

    void handleIncomingCall(CallItem *call);
    void handleCallsChanged();
    void handleResourceUnavailability(const QString);

    void call(QString no);
    void accept();

    void showUi();
    void hideUi();

    int  showErrorDialog(const QString msg);
    int  showErrorDialog();
    void showDebugPage();
    void showTBD();

    void simulateIncomingCall();

protected:
    void setupUi();

    virtual void closeEvent(QCloseEvent *event);

    QStringList dumpDisplayInfo();

private Q_SLOTS:
    void callStateChanged();
    void onDisplayEntered();

private:
    MainWindow();

    MGConfItem         *m_configLastPage;

    MButtonGroup       *m_header;
    MApplicationPage   *m_lastPage;
    AlertDialog        *m_alert;
    NotificationDialog *m_notification;
    DialerKeyPad       *m_keypad;
    MRemoteAction       m_acceptAction;
    CallItem           *m_incomingCall;
    MMessageBox        *m_tbd;

    Q_DISABLE_COPY(MainWindow);
};

#endif // MAINWINDOW_H
