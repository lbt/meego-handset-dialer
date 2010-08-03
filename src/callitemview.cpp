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
#include "callitem.h"
#include "callitemview.h"
#include <MLabel>
#include <MImageWidget>
#include <MButton>
#include <QGraphicsGridLayout>
#include <QGraphicsSceneEvent>
#include <MCancelEvent>
#include <MSeparator>
#include <QTimer>
#include <QDebug>

CallItemView::CallItemView(CallItem *controller)
    : MWidgetView(controller),
      m_duration(new MLabel("00:00:00", controller)),
      m_status(new MLabel("...", controller)),
      m_picturePath(QString()),
      m_picture(QPixmap()),
      m_updateTimer(this)
{
    TRACE

    m_controller = controller;
    m_layout = new QGraphicsGridLayout;
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);
    m_controller->setLayout(m_layout);

    m_status->setAlignment(Qt::AlignCenter);

    if (peopleItem())
        peopleItem()->setObjectName("callItemPeopleItem");
    m_status->setObjectName("callItemStatusName");

    initLayout();

    connect(m_controller, SIGNAL(stateChanged()), SLOT(updateStatusLabel()));
}

CallItemView::~CallItemView()
{
    TRACE
}

/*
 * Getters...
 */
PeopleItem *CallItemView::peopleItem() const
{
    TRACE
    return m_controller->peopleItem(); //m_peopleItem;
}

MLabel *CallItemView::durationLabel() const
{
    TRACE
    return m_duration;
}

MLabel *CallItemView::statusLabel() const
{
    TRACE
    return m_status;
}

QGraphicsGridLayout *CallItemView::layout() const
{
    TRACE
    return m_layout;
}

/*
 * Setters...
 */
void CallItemView::setStatusLabel(const QString &text)
{
    TRACE
    statusLabel()->setText(text);
}

void CallItemView::clearLayout()
{
    for(int i=0;i<m_layout->count();i++)
        m_layout->removeAt(0);
}

void CallItemView::initLayout()
{
    TRACE
    static uint seed = 0;
    if (seed == 0)
        qsrand(QDateTime::currentDateTime().toTime_t());

    clearLayout();

    MWidget *spacer = new MWidget();
    spacer->setObjectName("callItemCenterSpacer");
    spacer->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::Expanding));
    durationLabel()->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                                               QSizePolicy::Expanding));
    durationLabel()->setAlignment(Qt::AlignRight);
    statusLabel()->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                                             QSizePolicy::Expanding));
    statusLabel()->setAlignment(Qt::AlignRight);
    if (peopleItem()) {
        peopleItem()->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                                QSizePolicy::Expanding));
        layout()->addItem(peopleItem(),0, 0, 1, 3, Qt::AlignTop|Qt::AlignLeft);
    } else {
        MLabel *lineid = new MLabel(m_controller->lineID());
        layout()->addItem(lineid,  0, 0, 1, 3, Qt::AlignTop|Qt::AlignLeft);
    }
    layout()->addItem(spacer,          1, 0, 1, 1, Qt::AlignCenter);
    layout()->addItem(statusLabel(),   1, 1, 1, 1, Qt::AlignRight);
    layout()->addItem(durationLabel(), 1, 2, 1, 1, Qt::AlignRight);

    QString thumb = "";
    if (!thumb.isEmpty()) {
        m_picturePath = QString("%1/%2/%3/%4").arg(THEMEDIR)
                                              .arg(M_APPLICATION_NAME)
                                              .arg("images/people/fullsize")
                                              .arg(thumb);
    }

    m_updateTimer.start(1000);
    connect(&m_updateTimer, SIGNAL(timeout()), SLOT(updateDurationLabel()));
}

void CallItemView::updateDurationLabel()
{
    QTime t = QTime(0,0).addSecs(m_controller->duration()/1000);
    durationLabel()->setText(t.toString(Qt::TextDate));
}

void CallItemView::updateStatusLabel()
{
    if (!model()->state().isEmpty())
        statusLabel()->setText(model()->state());
}

void CallItemView::applyStyle()
{
    TRACE
    MWidgetView::applyStyle();

    if (peopleItem())
        peopleItem()->setObjectName(style()->peopleItemObjectName());
    durationLabel()->setObjectName(style()->durationObjectName());
    statusLabel()->setObjectName(style()->statusObjectName());
    if (!m_picturePath.isEmpty()) {
        QRect s = QRect(QPoint(0,0),sizeHint(Qt::PreferredSize).toSize());
        m_picture = QPixmap(m_picturePath).scaledToWidth(s.width()).copy(s);
    }
}

void CallItemView::drawBackground(QPainter* painter,
                                  const QStyleOptionGraphicsItem* option) const
{
    Q_UNUSED(option);
    if (!m_picture.isNull())
        painter->drawPixmap(0,0,m_picture);
}

void CallItemView::updateData(const QList<const char *> &modifications)
{
    TRACE
qDebug() << QString("Model data changed...");
    MWidgetView::updateData(modifications);

    const char* member;
    for (int i=0; i<modifications.count(); i++) {
        member = modifications[i];
        qDebug() << QString("\"%1\" element changed in model").arg(member);
/*
        if (member == CallItemModel::Name) {
            setName(model()->name());
        } else if (member == CallItemModel::Phone) {
            setPhone(model()->phone());
        } else if (member == CallItemModel::LastCommTime) {
            setLastComm(model()->lastCommTime());
        } else if(member == CallItemModel::LastCommType){
            setLastCommIcon(model()->lastCommType());
        } else if(member == CallItemModel::Presence){
            setPresenceIcon(model()->presence());
        } else if (member == CallItemModel::Photo) {
            setPhoto(model()->photo());
        } else if (member == CallItemModel::LayoutType) {
            initLayout(model()->layoutType());
        } else if (member == CallItemModel::Selected) {
            setSelected(model()->selected());
        } else if (member == CallItemModel::Favorite) {
            setFavoriteIcon((model()->favorite())?"favourite":"normal");
        }
*/
    }
}

void CallItemView::setupModel()
{
    TRACE

qDebug() << QString("Initial model data setup");
    MWidgetView::setupModel();

    if (!model()->state().isEmpty())
        updateStatusLabel();

/*
    if (!model()->phone().isEmpty())
        setPhone(model()->phone());

    setLastComm(model()->lastCommTime());

    if (!model()->lastCommType() == 0)
        setLastCommIcon(model()->lastCommType());

    if (!model()->presence() == 0)
        setPresenceIcon(model()->presence());

    if (!model()->photo().isEmpty())
        setPhoto(model()->photo());

    setFavoriteIcon((model()->favorite())?"favourite":"normal");

    setSelected(model()->selected());
*/

    initLayout();

}

void CallItemView::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    TRACE

    m_pressed = true;
    setSelected(model()->selected());
    ev->accept();
}

void CallItemView::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
{
    TRACE

    if (m_pressed) {
        ev->accept();
        m_pressed = false;
        m_controller->click();
    } else
        ev->ignore();
}

void CallItemView::cancelEvent(MCancelEvent *ev)
{
    TRACE
    m_pressed = false;
    setSelected(model()->selected());
    ev->accept();
}

void CallItemView::setSelected(bool selected)
{
    TRACE
    if (selected)
        style().setModeSelected();
    else if (m_pressed)
        style().setModePressed();
    else
        style().setModeDefault();

    update();
}

QString CallItemView::generateDateTime()
{
    TRACE
    QDateTime now = QDateTime::currentDateTime();
    now = now.addDays(qrand() % 7 * -1);
    now = now.addSecs(qrand() % 3000);
    return now.toString();
}

QString CallItemView::generateCommType()
{
    TRACE
    int direction(qrand() % PeopleItem::COMM_LAST);
    if (direction <= PeopleItem::COMM_NONE)
        direction = 1;
    else if (direction >= PeopleItem::COMM_LAST)
        direction = PeopleItem::COMM_LAST - 1;

    return QString::number(direction);
}

QString CallItemView::generatePresenceType()
{
    TRACE
    int presence(qrand() % PeopleItem::PRESENCE_LAST);
    if (presence <= PeopleItem::PRESENCE_NONE)
        presence = 1;
    else if (presence >= PeopleItem::PRESENCE_LAST)
        presence = PeopleItem::PRESENCE_LAST - 1;

    return QString::number(presence);
}
M_REGISTER_VIEW(CallItemView, CallItem)
