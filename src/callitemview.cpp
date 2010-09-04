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
#include <MScalableImage>
#include <MButton>
#include <QGraphicsGridLayout>
#include <QGraphicsSceneEvent>
#include <MCancelEvent>
#include <MSeparator>
#include <QTimer>
#include <QDebug>

CallItemView::CallItemView(CallItem *controller)
    : MWidgetView(controller),
      //% "00:00:00"
      m_duration(new MLabel(qtTrId("xx_default_duration"), controller)),
      //% "..."
      m_status(new MLabel(qtTrId("xx_default_status"), controller)),
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
    TRACE
    if (!model()->state().isEmpty())
        statusLabel()->setText(model()->stateTr());

    if (peopleItem())
        peopleItem()->setSelected(false);

    switch (model()->stateType()) {
    case CallItemModel::STATE_ACTIVE:
        qDebug() << QString("CallItemView: setModeActive()");
        style().setModeActive();
        if (peopleItem())
            peopleItem()->setSelected(true);
        break;
    case CallItemModel::STATE_HELD:
        qDebug() << QString("CallItemView: setModeHeld()");
        style().setModeHeld();
        break;
    case CallItemModel::STATE_DIALING:
        qDebug() << QString("CallItemView: setModeDialing()");
        style().setModeDialing();
        break;
    case CallItemModel::STATE_ALERTING:
        qDebug() << QString("CallItemView: setModeAlerting()");
        style().setModeAlerting();
        break;
    case CallItemModel::STATE_INCOMING:
        qDebug() << QString("CallItemView: setModeIncoming()");
        style().setModeIncoming();
        break;
    case CallItemModel::STATE_WAITING:
        qDebug() << QString("CallItemView: setModeWaiting()");
        style().setModeWaiting();
        break;
    case CallItemModel::STATE_DISCONNECTED:
        qDebug() << QString("CallItemView: setModeDisconnected()");
        style().setModeDisconnected();
        break;
    default:
        qDebug("CallItemView: setModeDefault()");
        style().setModeDefault();
        break;
    }

    applyStyle();
}

void CallItemView::applyStyle()
{
    TRACE
    MWidgetView::applyStyle();

    if (peopleItem())
        peopleItem()->setObjectName(style()->peopleItemObjectName());
    durationLabel()->setObjectName(style()->durationObjectName());
    statusLabel()->setObjectName(style()->statusObjectName());

    update();
}

void CallItemView::drawBackground(QPainter* painter,
                                  const QStyleOptionGraphicsItem* option) const
{
    Q_UNUSED(option);
    qreal previousOpacity(painter->opacity());
    painter->setOpacity(style()->backgroundOpacity()*effectiveOpacity());

    // Draw backgroundImage if it exists, otherwise backgroundColor
    if (style()->backgroundImage())
        style()->backgroundImage()->draw(0, 0, size().width(), size().height(),
                                         painter);
    else if (style()->backgroundColor().isValid())
        painter->fillRect(QRectF(QPointF(0, 0), size()),
                          style()->backgroundColor());

    painter->setOpacity(previousOpacity);
}

void CallItemView::updateData(const QList<const char *> &modifications)
{
    TRACE
    MWidgetView::updateData(modifications);

    const char* member;
    for (int i=0; i<modifications.count(); i++) {
        member = modifications[i];
        qDebug() << QString("CallItemView::updateData(): %1 changed")
                    .arg(member);
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
        updateStatusLabel();
    }
}

void CallItemView::setupModel()
{
    TRACE

qDebug() << QString("Initial model data setup");
    MWidgetView::setupModel();

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

    updateStatusLabel();
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
