/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PEOPLEITEMMODEL_H
#define PEOPLEITEMMODEL_H

#include <QString>
#include <QDateTime>
#include <mwidgetmodel.h>

class M_EXPORT PeopleItemModel: public MWidgetModel
{
    Q_OBJECT
    M_MODEL(PeopleItemModel)
public:

private:
    M_MODEL_PROPERTY(QString, name, Name, true, QString())
    M_MODEL_PROPERTY(QString, phone, Phone, true, QString())
    M_MODEL_PROPERTY(QDateTime, lastCommTime, LastCommTime, true, QDateTime())
    M_MODEL_PROPERTY(int, lastCommType, LastCommType, true, 0)
    M_MODEL_PROPERTY(int, presence, Presence, true, 0)
    M_MODEL_PROPERTY(QString, photo, Photo, true, QString())
    M_MODEL_PROPERTY(int, layoutType, LayoutType, true, 3)
    M_MODEL_PROPERTY(bool, selected, Selected, true, false)
    M_MODEL_PROPERTY(bool, favorite, Favorite, true, false)
};

#endif // PEOPLEITEMMODEL_H
