/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "historytablemodel.h"
#include "dialerapplication.h"
#include "managerproxy.h"

static QString historyHeaderLabels[] = {
    "LineID",
    "Direction",
    "CallStart",
    "CallEnd",
};

HistoryTableModel::HistoryTableModel()
{
    TRACE

    for (int i=0; i<COLUMN_LAST; i++)
        m_headers << (historyHeaderLabels[i]);

    insertRows(0, -1); // -1 == special case, load all
}

HistoryTableModel::~HistoryTableModel()
{
    TRACE
}

int HistoryTableModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    //Each entry in the QVector is a row
    return m_data.size();
}

int HistoryTableModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    if (!m_data.size())
        return 0;

    //Each row (including the first) is a QStringList, return it's size
    return m_data.first().size();
}

QVariant HistoryTableModel::data(const QModelIndex & index, int role) const
{
    TRACE

    Q_ASSERT(index.isValid());
    Q_ASSERT(index.row() < m_data.size());
    Q_ASSERT(index.column() < m_data.first().size());

    int row = index.row();
    int col = index.column();

    switch (role) {
    case Qt::DisplayRole:
    case DisplayRole:
    case SortRole:
        if (index.isValid() && index.row() < m_data.size()) {
            return QVariant::fromValue(m_data.at(row).at(col));
        }
        break;
    case CompleterRole:
        if (index.isValid() && index.row() < m_data.size() &&
           (col == COLUMN_LINEID)) {
            return QVariant::fromValue(stripLineID(m_data.at(row).at(col)));
        }
        break;
    default:
        break;
    }
    return QVariant();
}

QVariant HistoryTableModel::headerData(int section,
                                       Qt::Orientation orientation,
                                       int role) const
{
    TRACE
    Q_UNUSED(orientation);

    Q_ASSERT(section < m_headers.size());

    if (role == Qt::DisplayRole) {
        if (section < m_headers.size()) {
            return QVariant(m_headers.at(section));
        }
    }
    return QVariant();
}

bool HistoryTableModel::insertRows(int row, int count,
                                   const QModelIndex & parent)
{
    TRACE

    int max = count;
    int i = 0;
    QStringList newRow = QStringList();
    ManagerProxy *mp = ManagerProxy::instance();
    HistoryProxy* historyProxy = mp->history();
    if(!historyProxy) return false; //abort
    QSettings *cache = historyProxy->cache();

    cache->beginGroup("CallHistory");
    QStringList events = cache->childGroups();

    if (events.size() == 0) {
        qWarning() << QString("[HistoryTableModel] Empty call history log!");
        return true;
    }

    // Special case to just load all data from cache
    if (max < 0)
        max = events.size();

    beginInsertRows(parent, row, row + max - 1);
    foreach (QString key, events) {

        // Stop before end of cache if we hit count
        if (i > max) break;

        cache->beginGroup(key);

        uint start = cache->value("Start").toUInt(); // Call start time
        uint end   = cache->value("End").toUInt();   // Call end time

        // add the column data to a new row
        newRow.clear();
        newRow << cache->value("LineID").toString(); // Phone Number
        newRow << cache->value("Type").toString();   // Call direction type
        newRow << toOfonoString(start);              // Call start time
        newRow << toOfonoString(end);                // Call end time

#ifdef WANT_DEBUG
        qDebug() << QString("[HistoryTableModel] Appending row: %1")
                           .arg(newRow.join("\t"));
#endif
        // add the row data to the vector
        m_data.append(newRow);
        cache->endGroup();
        i++;
    }
    cache->endGroup();
    endInsertRows();
    return true;
}

void HistoryTableModel::appendRows(QStringList keys)
{
    TRACE

    int max = 0;
    int i = 0;
    QStringList newRow = QStringList();
    ManagerProxy *mp = ManagerProxy::instance();
    QSettings *cache = mp->history()->cache();

    cache->beginGroup("CallHistory");
    QStringList events = cache->childGroups();

    if (events.size() == 0) {
        qWarning() << QString("[HistoryTableModel] Empty call history log!");
        return;
    }

    // Special case to just load all data from cache
    max = keys.size();

    beginInsertRows(QModelIndex(), rowCount(), rowCount() + max - 1);
    foreach (QString key, keys) {

        //if (!cache->contains(key)) continue;  // Skip items not in cache

        cache->beginGroup(key);

        uint start = cache->value("Start").toUInt(); // Call start time
        uint end   = cache->value("End").toUInt();   // Call end time

        // add the column data to a new row
        newRow.clear();
        newRow << cache->value("LineID").toString(); // Phone Number
        newRow << cache->value("Type").toString();   // Call direction type
        newRow << toOfonoString(start);              // Call start time
        newRow << toOfonoString(end);                // Call end time

#ifdef WANT_DEBUG
        qDebug() << QString("[HistoryTableModel] Appending row: %1")
                           .arg(newRow.join("\t"));
#endif
        // add the row data to the vector
        m_data.append(newRow);
        cache->endGroup();
        i++;
    }
    cache->endGroup();
    endInsertRows();
    return;
}

bool HistoryTableModel::removeRows(int row, int count,
                                   const QModelIndex & parent)
{
    TRACE

    beginRemoveRows(parent, row, row + count - 1);
    m_data.remove(row, count);
    endRemoveRows();
    return true;
}

QString HistoryTableModel::toOfonoString(QDateTime when)
{
    TRACE
    return when.toString(Qt::ISODate);
}

QString HistoryTableModel::toOfonoString(uint t)
{
    TRACE
    return toOfonoString(QDateTime::fromTime_t(t));
}

QDateTime HistoryTableModel::generateQDateTime()
{
    TRACE
    QDateTime now = QDateTime::currentDateTime();
    now = now.addDays(qrand() % 7 * -1);
    now = now.addSecs(qrand() % 3000);
    return now;
}

QString HistoryTableModel::generateDateTime()
{
    TRACE
    return toOfonoString(generateQDateTime());
}

QString HistoryTableModel::generateEndTime(QDateTime start)
{
    TRACE
    QDateTime end = start.addSecs(qrand() % 600);  // Add up to 10min
    return toOfonoString(end);
}

QString HistoryTableModel::generateDirection()
{
    TRACE
    int dir(qrand() % 2);  // 0=OUT, 1=IN, 2=MISS

    return QString::number(dir);
}
