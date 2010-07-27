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
 
    PeopleItem *card = qobject_cast<PeopleItem *>(cell);

    SEASIDE_SHORTCUTS
    SEASIDE_SET_MODEL_AND_ROW(index.model(),index.row());

    card->setName(QString("%1, %2").arg(SEASIDE_FIELD(LastName, String))
                                   .arg(SEASIDE_FIELD(FirstName, String)));
    card->setPhoto(SEASIDE_FIELD(Avatar, String));
    card->setLastCommTime(SEASIDE_FIELD(CommTimestamp,DateTime));
    card->setPresence((Seaside::Presence)SEASIDE_FIELD(Presence,Int));
    card->setFavorite((SEASIDE_FIELD(Favorite,Bool))?"1":"0");

    // TODO: Figure out *which* phone number to show...
    QStringList list = SEASIDE_FIELD(PhoneNumbers,StringList);
    card->setPhone((list.isEmpty())?"":list.at(0));

    // TODO: SeasideSyncModel does not contain last communication type, will
    //        need to get this from CallHistory or Seaside model when/if it
    //        materializes.
#if 0
    card->setLastCommType((PeopleItem::CommType)SEASIDE_FIELD(CommType,Int));
#else
    card->setLastCommType((PeopleItem::CommType)
                          (qMax(1,qrand() % (PeopleItem::COMM_LAST-1))));
#endif
}
