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
#include "peopleitemview.h"
#include <seaside.h>
#include <MLabel>
#include <MImageWidget>
#include <QGraphicsGridLayout>
#include <QGraphicsSceneEvent>
#include <MCancelEvent>
#include <QDebug>

PeopleItemView::PeopleItemView(PeopleItem *controller)
    : MWidgetView(controller),
      m_name(NULL), m_phone(NULL), m_lastComm(NULL), m_lastCommIcon(NULL),
      m_presenceIcon(NULL), m_photo(NULL), m_favoriteIcon(NULL),
      m_layoutType(PeopleItem::LAYOUT_SINGLE_LINE),
      m_pressed(false)
{
    TRACE

    m_controller = controller;

    m_layout = new QGraphicsGridLayout;
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_controller->setLayout(m_layout);

    m_name = new MLabel(m_controller);
    m_name->setAlignment(Qt::AlignLeft);
    m_phone = new MLabel(m_controller);
    m_phone->setAlignment(Qt::AlignLeft);
    m_lastComm = new MLabel(m_controller);
    m_lastComm->setAlignment(Qt::AlignRight);
    m_lastCommIcon = new MImageWidget(m_controller);
    m_presenceIcon = new MImageWidget(m_controller);
    m_photo = new MImageWidget(m_controller);
    m_favoriteIcon = new MImageWidget(m_controller);

    m_name->setObjectName("peopleItemName");
    m_phone->setObjectName("peopleItemPhone");
    m_lastComm->setObjectName("peopleItemLastComm");
    m_lastCommIcon->setObjectName("peopleItemLastCommIcon");
    m_presenceIcon->setObjectName("peopleItemPresenceIcon");
    m_photo->setObjectName("peopleItemPhoto");
    m_favoriteIcon->setObjectName("peopleItemFavoriteIcon");

    initLayout(m_layoutType);
    m_layoutType = PeopleItem::LAYOUT_DOUBLE_LINE;
}

PeopleItemView::~PeopleItemView()
{
    TRACE
}

/*
 * Getters...
 */
MLabel *PeopleItemView::name() const
{
    TRACE
    return m_name;
}

MLabel *PeopleItemView::phone() const
{
    TRACE
    return m_phone;
}

MLabel *PeopleItemView::lastComm() const
{
    TRACE
    return m_lastComm;
}

MImageWidget *PeopleItemView::lastCommIcon() const
{
    TRACE
    return m_lastCommIcon;
}

MImageWidget *PeopleItemView::presenceIcon() const
{
    TRACE
    return m_presenceIcon;
}

MImageWidget *PeopleItemView::photo() const
{
    TRACE
    return m_photo;
}

MImageWidget *PeopleItemView::favoriteIcon() const
{
    TRACE
    return m_favoriteIcon;
}

QGraphicsGridLayout *PeopleItemView::layout() const
{
    TRACE
    return m_layout;
}

/*
 * Setters...
 */
void PeopleItemView::setName(const QString &text)
{
    TRACE
    if (text.isEmpty() || text.isNull())
        name()->setText("Unknown");
    else
        name()->setText(text);
}

void PeopleItemView::setPhone(const QString &text)
{
    TRACE
    if (text.isEmpty() || text.isNull())
        phone()->setText(" ");
    else
        phone()->setText(text);
}

void PeopleItemView::setLastComm(const QString &text)
{
    TRACE
    if (text.isEmpty() || text.isNull())
        lastComm()->setText(" ");
    else
        lastComm()->setText(text);
}

void PeopleItemView::setLastComm(const QDateTime &dt)
{
    TRACE
    setLastComm(dateToFriendlyString(dt.date()));
}

void PeopleItemView::setLastCommIcon(const QString &id)
{
    TRACE
    if (id.isEmpty() || id.isNull())
        lastCommIcon()->setImage("none");
    else
        lastCommIcon()->setImage(id);
}

void PeopleItemView::setLastCommIcon(const int &ct)
{
    TRACE
    QString id;
    switch (static_cast<PeopleItem::CommType>(ct)) {
        case PeopleItem::COMM_CALL_DIALED:
            id = QString("icon-m-telephony-call-initiated");
            break;
        case PeopleItem::COMM_CALL_RECEIVED:
            id = QString("icon-m-telephony-call-received");
            break;
        case PeopleItem::COMM_CALL_MISSED:
            id = QString("icon-m-telephony-call-missed");
            break;
        case PeopleItem::COMM_CALL_UNANSWERED:
            id = QString("icon-m-telephony-call-missed");
            break;
        case PeopleItem::COMM_SMS_SENT:
            id = QString("small-sms-sent");
            break;
        case PeopleItem::COMM_SMS_RECEIVED:
            id = QString("small-sms-recieved");
            break;
        case PeopleItem::COMM_EMAIL_SENT:
            id = QString("small-email-sent");
            break;
        case PeopleItem::COMM_EMAIL_RECEIVED:
            id = QString("small-email-recieved");
            break;
        default:
            id = QString("none");
            break;
    }
    setLastCommIcon(id);
}

void PeopleItemView::setPresenceIcon(const QString &id)
{
    TRACE
    if (id.isEmpty() || id.isNull())
        presenceIcon()->setImage("icon-m-common-presence-unknown");
    else
        presenceIcon()->setImage(id);
}

void PeopleItemView::setPresenceIcon(const int &pt)
{
    TRACE
    QString id;
    switch (static_cast<Seaside::Presence>(pt)) {
        case Seaside::PresenceAvailable:
            id = QString("icon-m-common-presence-online");
            break;
/* Uncomment when "Busy" is available from Seaside
        case Seaside::PresenceBusy:
            id = QString("icon-busy");
            break;
*/
        case Seaside::PresenceAway:
            id = QString("icon-m-common-presence-away");
            break;
        case Seaside::PresenceOffline:
            id = QString("icon-m-common-presence-offline");
            break;
        default:
            id = QString("icon-m-common-presence-unknown");
            break;
    }
    setPresenceIcon(id);
}

void PeopleItemView::setPhoto(const QString &id)
{
    TRACE
    if (id.isNull() || id.isEmpty())
        photo()->setImage("icon-m-content-avatar-placeholder");
    else
        photo()->setImage(id);
}

void PeopleItemView::setFavoriteIcon(const QString &id)
{
    TRACE
    if (id.isNull() || id.isEmpty())
        favoriteIcon()->setImage("normal");
    else
        favoriteIcon()->setImage(id);
}

void PeopleItemView::clearLayout()
{
    for(int i=0;i<m_layout->count();i++)
        m_layout->removeAt(0); 
}

void PeopleItemView::initLayout(int layoutType)
{
    initLayout(static_cast<PeopleItem::LayoutType>(layoutType));
}

void PeopleItemView::initLayout(PeopleItem::LayoutType layoutType)
{
    TRACE

    if (m_layoutType == layoutType)
        return;

    clearLayout();

    m_layoutType = layoutType;

    switch(layoutType) {
    case PeopleItem::LAYOUT_SINGLE_LINE:
        lastCommIcon()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                                                  QSizePolicy::Fixed));
        presenceIcon()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                                                  QSizePolicy::Fixed));
        layout()->addItem(name(),         0, 0, Qt::AlignRight);
        layout()->addItem(favoriteIcon(), 0, 1, Qt::AlignRight);
        layout()->addItem(presenceIcon(), 0, 2, Qt::AlignRight);
        layout()->addItem(lastComm(),     0, 3, Qt::AlignRight);
        layout()->addItem(lastCommIcon(), 0, 4, Qt::AlignRight);

        name()->setVisible(true);
        phone()->setVisible(false);
        lastComm()->setVisible(true);
        lastCommIcon()->setVisible(true);
        presenceIcon()->setVisible(true);
        photo()->setVisible(false);
        favoriteIcon()->setVisible(true);
        break;

    case PeopleItem::LAYOUT_PHOTO_AND_SINGLE_LINE:
        lastCommIcon()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                                                  QSizePolicy::Fixed));
        presenceIcon()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                                                  QSizePolicy::Fixed));
        photo()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                                           QSizePolicy::Fixed));
        layout()->addItem(photo(),        0, 0, Qt::AlignCenter);
        layout()->addItem(name(),         0, 1, Qt::AlignRight);
        layout()->addItem(favoriteIcon(), 0, 2, Qt::AlignRight);
        layout()->addItem(presenceIcon(), 0, 3, Qt::AlignRight);
        layout()->addItem(lastComm(),     1, 4, Qt::AlignRight);
        layout()->addItem(lastCommIcon(), 1, 5, Qt::AlignRight);

        name()->setVisible(true);
        phone()->setVisible(false);
        lastComm()->setVisible(true);
        lastCommIcon()->setVisible(true);
        presenceIcon()->setVisible(true);
        photo()->setVisible(true);
        favoriteIcon()->setVisible(true);
        break;

    case PeopleItem::LAYOUT_DOUBLE_LINE:
        lastCommIcon()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                                                  QSizePolicy::Fixed));
        presenceIcon()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                                                  QSizePolicy::Fixed));
        layout()->addItem(name(),         0, 0, Qt::AlignRight);
        layout()->addItem(favoriteIcon(), 0, 1, Qt::AlignRight);
        layout()->addItem(presenceIcon(), 0, 2, Qt::AlignRight);
        layout()->addItem(phone(),        1, 0, Qt::AlignRight);
        layout()->addItem(lastComm(),     1, 1, Qt::AlignRight);
        layout()->addItem(lastCommIcon(), 1, 2, Qt::AlignRight);

        name()->setVisible(true);
        phone()->setVisible(true);
        lastComm()->setVisible(true);
        lastCommIcon()->setVisible(true);
        presenceIcon()->setVisible(true);
        photo()->setVisible(false);
        favoriteIcon()->setVisible(true);
        break;

    case PeopleItem::LAYOUT_NONE:
    case PeopleItem::LAYOUT_LAST:
    case PeopleItem::LAYOUT_PHOTO_AND_DOUBLE_LINE:
    default:
        lastCommIcon()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                                                  QSizePolicy::Fixed));
        presenceIcon()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                                                  QSizePolicy::Fixed));
        photo()->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                                           QSizePolicy::Fixed));
        layout()->addItem(photo(),        0, 0, 2, 1, Qt::AlignCenter);
        layout()->addItem(name(),         0, 1, Qt::AlignRight);
        layout()->addItem(favoriteIcon(), 0, 2, Qt::AlignRight);
        layout()->addItem(presenceIcon(), 0, 3, Qt::AlignRight);
        layout()->addItem(phone(),        1, 1, Qt::AlignRight);
        layout()->addItem(lastComm(),     1, 2, Qt::AlignRight);
        layout()->addItem(lastCommIcon(), 1, 3, Qt::AlignRight);

        name()->setVisible(true);
        phone()->setVisible(true);
        lastComm()->setVisible(true);
        lastCommIcon()->setVisible(true);
        presenceIcon()->setVisible(true);
        photo()->setVisible(true);
        favoriteIcon()->setVisible(true);
        break;
    };
    setSelected(model()->selected());
}

void PeopleItemView::applyStyle()
{
    TRACE

    MWidgetView::applyStyle();

    name()->setObjectName(style()->nameObjectName());
    phone()->setObjectName(style()->phoneObjectName());
    lastComm()->setObjectName(style()->lastCommObjectName());
    lastCommIcon()->setObjectName(style()->lastCommIconObjectName());
    presenceIcon()->setObjectName(style()->presenceObjectName());
    photo()->setObjectName(style()->photoObjectName());
    favoriteIcon()->setObjectName(style()->favoriteIconObjectName());
}

QString PeopleItemView::dateToFriendlyString(const QDate &date)
{
    TRACE
    if (!date.isValid())
        return QString();
    if (date == QDate::currentDate())
        // FIXME: Needs translation
        return QString("Today");
    else if (date == QDate::currentDate().addDays(-1))
        // FIXME: Needs translation
        return QString("Yesterday");
    else if (date >= QDate::currentDate().addDays(-5))
        return date.longDayName(date.dayOfWeek());
    else
        return date.toString(Qt::SystemLocaleShortDate);
}

void PeopleItemView::updateData(const QList<const char *> &modifications)
{
    TRACE

    MWidgetView::updateData(modifications);

    const char* member;
    for (int i=0; i<modifications.count(); i++) {
        member = modifications[i];
        if (member == PeopleItemModel::Name) {
            setName(model()->name());
        } else if (member == PeopleItemModel::Phone) {
            setPhone(model()->phone());
        } else if (member == PeopleItemModel::LastCommTime) {
            setLastComm(model()->lastCommTime());
        } else if(member == PeopleItemModel::LastCommType){
            setLastCommIcon(model()->lastCommType());
        } else if(member == PeopleItemModel::Presence){
            setPresenceIcon(model()->presence());
        } else if (member == PeopleItemModel::Photo) {
            setPhoto(model()->photo());
        } else if (member == PeopleItemModel::LayoutType) {
            initLayout(model()->layoutType());
        } else if (member == PeopleItemModel::Selected) {
            setSelected(model()->selected());
        } else if (member == PeopleItemModel::Favorite) {
            setFavoriteIcon((model()->favorite())?
                             "icon-m-common-favorite-mark":"normal");
/*
            if (model()->favorite())
                favoriteIcon()->setVisible(true);
            else
                favoriteIcon()->setVisible(false);
*/
        }
    }
}

void PeopleItemView::setupModel()
{
    TRACE

    MWidgetView::setupModel();

    if (!model()->name().isEmpty())
        setName(model()->name());

    if (!model()->phone().isEmpty())
        setPhone(model()->phone());

    setLastComm(model()->lastCommTime());

    if (!model()->lastCommType() == 0)
        setLastCommIcon(model()->lastCommType());

    if (!model()->presence() == 0)
        setPresenceIcon(model()->presence());

    if (!model()->photo().isEmpty())
        setPhoto(model()->photo());

    setFavoriteIcon((model()->favorite())?
                     "icon-m-common-favorite-mark":"normal");

    setSelected(model()->selected());

    initLayout(static_cast<PeopleItem::LayoutType>(model()->layoutType()));

}

void PeopleItemView::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    TRACE

    m_pressed = true;
    setSelected(model()->selected());
    ev->accept();
}

void PeopleItemView::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
{
    TRACE

    if (m_pressed) {
        ev->accept();
        m_pressed = false;
        m_controller->click();
    } else
        ev->ignore();
}

void PeopleItemView::cancelEvent(MCancelEvent *ev)
{
    TRACE
    m_pressed = false;
    setSelected(model()->selected());
    ev->accept();
}

void PeopleItemView::setSelected(bool selected)
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

M_REGISTER_VIEW(PeopleItemView, PeopleItem)
