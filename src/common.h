/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef COMMON_H
#define COMMON_H

#ifndef WANT_DEBUG
#define TRACE
#else
#include <QDebug>
#define TRACE qDebug()<<QString("[%1] %2(): %3").arg(__FILE__).arg(__func__).arg(__LINE__);
#endif

/*
 * Commonly used QRegExp expressions
 */
#include <QRegExp>
#define MATCH_ANY(p) QRegExp(p,Qt::CaseInsensitive,QRegExp::FixedString)
#define MATCH_ALL QRegExp()

/*
 * Commonly used values for MNotifications
 */
#define DEFAULT_AVATAR_ICON "icon-m-content-avatar-placeholder"
#define NOTIFICATION_CALL_EVENT "x-nokia.call"

/*
 * Convienence macro for showing TBD message
 */
#define SHOW_TBD \
    MainWindow *w = dynamic_cast<MainWindow *>(DialerApplication::instance()->activeApplicationWindow()); \
    if (w) \
       w->showTBD();



QString stripLineID(QString lineid);

bool currentPageIs(int pagenum);

#include <QDateTime>
QDateTime qDateTimeFromOfono(const QString &val);

#endif // COMMON_H
