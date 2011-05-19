/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef HISTORYTABLEMODEL_H
#define HISTORYTABLEMODEL_H

// Model providing sample contacts from hard-coded data

#include "common.h"
#include <QAbstractTableModel>
#include <QVector>
#include <QStringList>
#include <QDateTime>

class HistoryTableModel : public QAbstractTableModel
{
public:
    enum HistoryRole {
        CompleterRole = Qt::UserRole+1,
        DisplayRole,
        SortRole,
        FilterRole,
        PhoneRole,
        LineIDRole,
        DirectionRole,
        CallStartRole,
        CallEndRole
    };

    enum HistoryColumn {
        COLUMN_LINEID = 0,
        COLUMN_DIRECTION,
        COLUMN_CALLSTART,
        COLUMN_CALLEND,
 
        COLUMN_LAST,
    };

    HistoryTableModel();
    virtual ~HistoryTableModel();

    virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent=QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index,
                          int role=Qt::DisplayRole) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const;

    bool insertRows(int row,int count,const QModelIndex &parent=QModelIndex());
    bool removeRows(int row,int count,const QModelIndex &parent=QModelIndex());
    void appendRows(QStringList keys);

private:
    QString toOfonoString(QDateTime);
    QString toOfonoString(uint);
    QDateTime generateQDateTime();
    QString generateDateTime();
    QString generateEndTime(QDateTime);
    QString generateDirection();

    QVector<QStringList> m_data;
    QStringList m_headers;
};

#endif // HISTORYTABLEMODEL_H
