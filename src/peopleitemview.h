/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PEOPLEITEMVIEW_H
#define PEOPLEITEMVIEW_H

#include "peopleitemmodel.h"
#include "peopleitemstyle.h"
#include "peopleitem.h"
#include <MWidgetView>

class MLabel;
class MImageWidget;
class QGraphicsGridLayout;

class M_EXPORT PeopleItemView : public MWidgetView
{
    Q_OBJECT
    M_VIEW(PeopleItemModel, PeopleItemStyle)

public:
    PeopleItemView(PeopleItem *controller);
    virtual ~PeopleItemView();
    QString dateToFriendlyString(const QDate &date);

protected slots:
    virtual void updateData(const QList<const char *> &modifications);

protected:
    virtual void setupModel();
    virtual void applyStyle();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void cancelEvent(MCancelEvent *event);
            void setSelected(bool selected);

private:
    MLabel *name() const;
    MLabel *phone() const;
    MLabel *lastComm() const;
    MImageWidget *lastCommIcon() const;
    MImageWidget *presenceIcon() const;
    MImageWidget *photo() const;
    MImageWidget *favoriteIcon() const;
    QGraphicsGridLayout *layout() const;

    void setName(const QString &text);
    void setPhone(const QString &text);
    void setLastComm(const QString &text);
    void setLastComm(const QDateTime &dt);
    void setLastCommIcon(const QString &id);
    void setLastCommIcon(const int &ct);
    void setPresenceIcon(const QString &id);
    void setPresenceIcon(const int &pt);
    void setPhoto(const QString &id);
    void setFavoriteIcon(const QString &id);

    void initLayout(int layoutType);
    void initLayout(PeopleItem::LayoutType layoutType);
    void clearLayout();

private:
    PeopleItem          *m_controller;
    MLabel            *m_name;
    MLabel            *m_phone;
    MLabel            *m_lastComm;
    MImageWidget      *m_lastCommIcon;
    MImageWidget      *m_presenceIcon;
    MImageWidget      *m_photo;
    MImageWidget      *m_favoriteIcon;
    QGraphicsGridLayout *m_layout;
    PeopleItem::LayoutType m_layoutType;
    bool                 m_pressed;

    Q_DISABLE_COPY(PeopleItemView)
};

#endif // PEOPLEITEMVIEW_H
