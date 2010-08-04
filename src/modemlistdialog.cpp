/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "modemlistdialog.h"
#include "dialerapplication.h"
#include "managerproxy.h"
#include <QDebug>

#include <MWidgetCreator>
M_REGISTER_WIDGET(AlertDialog)

ModemListDialog::ModemListDialog()
    : MDialog("Phone Available:", M::NoStandardButton),
      m_panel(new MStylableWidget()),
      m_layout(new MLayout(m_panel)),
      m_land(new MGridLayoutPolicy(m_layout)),
      m_port(new MGridLayoutPolicy(m_layout)),
      m_cancel(new MButton("Back"))
{
    TRACE
    setCloseButtonVisible(false);
    init();
}

ModemListDialog::~ModemListDialog()
{
    TRACE
}

void ModemListDialog::init()
{
    TRACE
    m_panel->setObjectName("modemListPanel");
    m_cancel->setObjectName("modemListCancelButton");
    m_cancel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                        QSizePolicy::Preferred));

    connect(m_cancel, SIGNAL(clicked()), SLOT(cancelClicked()));
    addButton(m_cancel->model());

    ManagerProxy *mp = ManagerProxy::instance();

    if (mp->isValid())
    {
        QList<QString> ml = mp->getModemList();

        if (ml.isEmpty())
        {
            MLabel *label = new MLabel(QString("No modem found"));
            m_layout->setLandscapePolicy(m_land);
            m_land->addItem(label,  0, 0, Qt::AlignCenter);

            m_layout->setPortraitPolicy(m_port);
            m_port->addItem(label,  0, 0, Qt::AlignVCenter);
        }
        else
        {
            for (int i=0;i<ml.size();i++) {
                QString path = ml.at(i);
                QString display = "Connect to " + path;

                MButton *button = new MButton(display);
                button->setObjectName("modemPathButton");
                button->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                                  QSizePolicy::Preferred));

                // Add the button the list of
                buttons.insert(button, path);

                connect(button, SIGNAL(clicked()), SLOT(connectModemClicked()));
                m_layout->setLandscapePolicy(m_land);
                m_land->addItem(button,  i, 0, Qt::AlignCenter);

                m_layout->setPortraitPolicy(m_port);
                m_port->addItem(button,  i, 0, Qt::AlignVCenter);
            }
        }
    }

    setCentralWidget(m_panel);
}


void ModemListDialog::cancelClicked()
{
    TRACE
}

void ModemListDialog::connectModemClicked()
{
    TRACE
    QObject *sender_object = sender();
    if (sender_object)   {
        MButton *button = qobject_cast<MButton *>(sender_object);

        if (button) {
            QString path = buttons.value(button);
            ManagerProxy *mp = ManagerProxy::instance();
            if (mp->isValid() && mp->getModemList().contains(path))
            {
                emit modemSelected(path);
            }
        }
    }
}


