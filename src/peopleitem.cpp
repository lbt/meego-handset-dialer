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
#include "peopleitem.h"
#include "peopleitemmodel.h"
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MLinearLayoutPolicy>
#include <MLabel>
#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QDebug>
#include <QSortFilterProxyModel>
#include <seaside.h>
#include <seasidesyncmodel.h>

#include <MWidgetCreator>
M_REGISTER_WIDGET(PeopleItem)

PeopleItem::PeopleItem(MWidget *parent)
    : MWidgetController(new PeopleItemModel, parent)
{
    TRACE
}

PeopleItem::~PeopleItem()
{
    TRACE
}

QString PeopleItem::name() const
{
    TRACE
    return model()->name();
}

QString PeopleItem::phone() const
{
    TRACE
    return model()->phone();
}

QString PeopleItem::photo() const
{
    TRACE
    return model()->photo();
}

QDateTime PeopleItem::lastCommTime() const
{
    TRACE
    return model()->lastCommTime();
}

PeopleItem::CommType PeopleItem::lastCommType() const
{
    TRACE
    return static_cast<PeopleItem::CommType>(model()->lastCommType());
}

PeopleItem::PresenceType PeopleItem::presence() const
{
    TRACE
    return static_cast<PeopleItem::PresenceType>(model()->presence());
}

void PeopleItem::setName(const QString &value)
{
    TRACE
    if (value.isNull())
        return;

    if (model()->name().isNull() || model()->name().isEmpty())
        model()->setName(value);

    else if (model()->name() != value)
        model()->setName(value);
}

void PeopleItem::setPhone(const QString &value)
{
    TRACE
    if (value.isNull())
        return;

    if (model()->phone().isNull() || model()->phone().isEmpty())
        model()->setPhone(value);

    else if (model()->phone() != value)
        model()->setPhone(value);
}


void PeopleItem::setPhoto(const QString &path)
{
    TRACE
    if (path.isNull())
        return;

    if (model()->photo().isNull() || model()->photo().isEmpty())
        model()->setPhoto(path);

    else if (model()->photo() != path)
        model()->setPhoto(path);
}

void PeopleItem::setFavorite(const QString &id)
{
    TRACE
    if (!id.isNull() && !id.isEmpty() && (id == "1"))
        model()->setFavorite(true);
    else
        model()->setFavorite(false);
}

void PeopleItem::setLastCommTime(const QDateTime &newTime)
{
    TRACE
    model()->setLastCommTime(newTime);
}

void PeopleItem::setLastCommType(PeopleItem::CommType commType)
{
    TRACE
    model()->setLastCommType(static_cast<int>(commType));
}

void PeopleItem::setLastCommType(int commType)
{
    TRACE
    model()->setLastCommType(commType);
}

void PeopleItem::setPresence(PeopleItem::PresenceType presence)
{
    TRACE
    model()->setPresence(static_cast<int>(presence));
}

void PeopleItem::setPresence(int presence)
{
    TRACE
    model()->setPresence(presence);
}

void PeopleItem::click()
{
    TRACE

    emit clicked();
}

QVariant PeopleItem::itemChange(GraphicsItemChange change, const QVariant &val)
{
    TRACE
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        model()->setSelected(val.toBool());

    return QGraphicsItem::itemChange(change, val);
}

/*
 * Cell Creator class implimentation
 */
PeopleItemCellCreator::PeopleItemCellCreator()
{
//    setCellObjectName("matchListItem");
}

void PeopleItemCellCreator::updateCell(const QModelIndex& index,
                                             MWidget * cell) const
{
    TRACE

    if (!cell)
        return;

    Q_ASSERT(index.isValid());
 
    static uint seed = 0;
    if (seed == 0)
        qsrand(QDateTime::currentDateTime().toTime_t());
 
    SeasideListItem *card = qobject_cast<SeasideListItem *>(cell);

    SEASIDE_SHORTCUTS
    SEASIDE_SET_MODEL_AND_ROW(index.model(),index.row());
    QString firstName = SEASIDE_FIELD(FirstName, String);
    QString lastName = SEASIDE_FIELD(LastName, String);

    if (lastName.isEmpty())
        // Contacts first (common) name
        //% "%1"
        card->setName(qtTrId("xx_first_name").arg(firstName));
    else
        // Contacts full, sortable name, defaults to "Lastname, Firstname"
        //% "%1, %2"
        card->setName(qtTrId("xx_full_name").arg(lastName).arg(firstName));
    card->setUuid(SEASIDE_FIELD(Uuid, String));
    card->setThumbnail(SEASIDE_FIELD(Avatar, String));
    card->setCommFlags(SEASIDE_FIELD(CommType,Int));
    card->setPresence(SEASIDE_FIELD(Presence,Int));
    card->setFavorite(SEASIDE_FIELD(Favorite,Bool));
    QStringList details = SEASIDE_FIELD(PhoneNumbers, StringList);
    // make sure that if the PhoneNumbers list is empty, we set Details field
    // to something that has actual empty ("") strings, otherwise we get other
    // phone numbers as artifacts of the widget recyler
    if (details.length() <= 0) details << QString("");
    card->setDetails(details);
    card->setButton("icon-m-telephony-call");
}
