/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "dialerapplication.h"
#include <QtGui>
#include <QApplication>
#include <MApplication>

int main(int argc, char *argv[])
{
    MApplicationService *service = new MApplicationService(DBUS_SERVICE);
    DialerApplication a(argc, argv, service);

    if (!a.isConnected()) {
        if (a.showErrorDialog() == M::AbortButton)
            qFatal("Aborting on user request...");
        else
            qDebug() << QString("Error ignored, continuing...");
    }

    exit(a.exec());
}


QString stripLineID(QString lineid)
{
    TRACE
    static QRegExp rx = QRegExp("([^0-9*#])");

    if (lineid.indexOf('+') == 0) {
        lineid.replace(rx, "");
        return lineid.insert(0,"+");
    }
    else
        return lineid.replace(rx, "");
}

bool currentPageIs(int pagenum)
{
    DialerApplication *app = DialerApplication::instance();
    MainWindow *mw = dynamic_cast<MainWindow *>(app->activeApplicationWindow());
    GenericPage *c = dynamic_cast<GenericPage *>(mw->currentPage());
    GenericPage *p = dynamic_cast<GenericPage *>(mw->m_pages.at(pagenum));

    return (c && p && (c == p));
}
