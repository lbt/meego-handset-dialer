/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef CALLITEMVIEW_H
#define CALLITEMVIEW_H

#include "callitemmodel.h"
#include "callitemstyle.h"
#include "callitem.h"
#include <MWidgetView>

class MLabel;
class MImageWidget;
class MButton;
class QGraphicsGridLayout;

class M_EXPORT CallItemView : public MWidgetView
{
    Q_OBJECT
    M_VIEW(CallItemModel, CallItemStyle)

public:
    CallItemView(CallItem *controller);
    virtual ~CallItemView();
    virtual void drawBackground(QPainter* painter,
                                const QStyleOptionGraphicsItem* option) const;
protected slots:
    virtual void updateDurationLabel();
    virtual void updateStatusLabel();
    virtual void updateData(const QList<const char *> &modifications);

protected:
    virtual void setupModel();
    virtual void applyStyle();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void cancelEvent(MCancelEvent *event);
            void setSelected(bool selected);

private:
    PeopleItem *peopleItem() const;
    MLabel *durationLabel() const;
    MLabel *statusLabel() const;
    QGraphicsGridLayout *layout() const;

    void setStatusLabel(const QString &text);

    void initLayout();
    void clearLayout();
    QString generateDateTime();
    QString generateCommType();
    QString generatePresenceType();

private:
    CallItem            *m_controller;
    MLabel              *m_duration;
    MLabel              *m_status;
    QGraphicsGridLayout *m_layout;
    bool                 m_pressed;
    QString              m_picturePath;
    QPixmap              m_picture;
    QTimer               m_updateTimer;

    Q_DISABLE_COPY(CallItemView)
};

#endif // CALLITEMVIEW_H
