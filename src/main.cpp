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

// Returns a valid QDateTime if parsable as such, otherwise the result
// will be !isValid()
QDateTime qDateTimeFromOfono(const QString &val)
{
    TRACE
    QDateTime result;

    if (val.isEmpty())
        return result;

    // NOTE: Ofono formats time to string with the following format spec:
    //       %Y-%m-%dT%H:%M:%S%z (for example: "2001-10-19T10:32:30-05:00")

    // Start by trying to parse this as an ISODate "YYYY-MM-DDTHH:MM:SSTZD"
    result = QDateTime::fromString(val,Qt::ISODate);
#ifdef WANT_DEBUG
    qDebug() << QString("Converted %1 with Qt::ISODate: %2")
                       .arg(val)
                       .arg(result.toString());
#endif

    if (!result.isValid()) {
    // ISODate conversion has failed, fallback to manual method
    // NOTE: QDateTime::fromString(val, Qt::ISODate) Fails since the date
    //       format from Ofono is in RFC 822 form, but QDateTime can't parse it
        struct tm time_tm;
        QByteArray  bytes = val.toAscii();
        const char *str = bytes.constData();
        if (strptime(str, "%Y-%m-%dT%H:%M:%S%z", &time_tm) != NULL) {
            time_t t = mktime(&time_tm);
            if (t >= (time_t)(0)) {
                result.setTime_t(t);
#ifdef WANT_DEBUG
                qDebug() << QString("Converted %1 with strptime: %2")
                                   .arg(val)
                                   .arg(result.toString());
#endif
            }
        }

        if (!result.isValid())
            qCritical() << QString("Format error, unknown date/time: %1")
                                  .arg(str);
    }

    return result;
}
