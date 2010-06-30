/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PEOPLEITEM_H
#define PEOPLEITEM_H

#include "peopleitemmodel.h"
#include <QString>
#include <QDateTime>
#include <MWidgetController>

class PeopleItem: public MWidgetController
{
    Q_OBJECT
    M_CONTROLLER(PeopleItem)

    enum CommType {
        COMM_NONE = 0,
        COMM_CALL_DIALED,
        COMM_CALL_RECEIVED,
        COMM_CALL_MISSED,
        COMM_CALL_UNANSWERED,
        COMM_SMS_SENT,
        COMM_SMS_RECEIVED,
        COMM_EMAIL_SENT,
        COMM_EMAIL_RECEIVED,
        COMM_LAST,
    };


    enum PresenceType {
        PRESENCE_NONE = 0,
        PRESENCE_AVAILABLE,
        PRESENCE_AWAY,
        PRESENCE_BUSY,
        PRESENCE_NOT_HERE,
        PRESENCE_LAST,
    };

    enum LayoutType {
        LAYOUT_NONE = 0,
        LAYOUT_DOUBLE_LINE,
        LAYOUT_SINGLE_LINE,
        LAYOUT_PHOTO_AND_DOUBLE_LINE,
        LAYOUT_PHOTO_AND_SINGLE_LINE,
        LAYOUT_LAST,
    };

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString phone READ phone WRITE setPhone)
    Q_PROPERTY(QString photo READ photo WRITE setPhoto)
    Q_PROPERTY(QDateTime lastCommTime READ lastCommTime WRITE setLastCommTime)
    Q_PROPERTY(CommType lastCommType READ lastCommType WRITE setLastCommType)
    Q_PROPERTY(PresenceType presence READ presence WRITE setPresence)

public:
    PeopleItem(MWidget *parent = 0);
    virtual ~PeopleItem();

    QString name() const;
    QString phone() const;
    QString photo() const;
    QDateTime lastCommTime() const;
    CommType lastCommType() const;
    PresenceType presence() const;

public Q_SLOTS:
    void setName(const QString &name);
    void setPhone(const QString &phone);
    void setPhoto(const QString &path);
    void setLastCommTime(const QDateTime &newTime);
    void setLastCommType(CommType commType);
    void setLastCommType(int commType);
    void setPresence(PresenceType presence);
    void setPresence(int presence);
    void setFavorite(const QString &id);
    void click();

Q_SIGNALS:
    // TODO: handle tap-and-hold
    void clicked();

private:
    QVariant itemChange(GraphicsItemChange change, const QVariant &val);

    Q_DISABLE_COPY(PeopleItem)
};

#include <mabstractcellcreator.h>

class PeopleItemCellCreator : public MAbstractCellCreator<PeopleItem>
{
public:
    PeopleItemCellCreator();
    void updateCell(const QModelIndex& index, MWidget * cell) const;
};

#endif // PEOPLEITEM_H
