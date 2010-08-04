/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "bluetoothdialog.h"
#include "modemlistdialog.h"
#include "dialerapplication.h"
#include "managerproxy.h"
#include <QDebug>

#include <MWidgetCreator>
M_REGISTER_WIDGET(AlertDialog)

BluetoothDialog::BluetoothDialog()
    : MDialog("Bluetooth:", M::NoStandardButton),
      m_panel(new MStylableWidget()),
      m_layout(new MLayout(m_panel)),
      m_land(new MGridLayoutPolicy(m_layout)),
      m_port(new MGridLayoutPolicy(m_layout)),
//      m_toggleBT(new MButton("On/Off Bluetooth")),
      m_cancel(new MButton("Close"))
{
    TRACE
    setCloseButtonVisible(false);
    init();
    connect(this, SIGNAL(appeared()), SLOT(hideKeypad()));
    connect(this, SIGNAL(disappeared()), SLOT(showKeypad()));
}

BluetoothDialog::~BluetoothDialog()
{
    TRACE
}

void BluetoothDialog::init()
{
    TRACE
    m_panel->setObjectName("bluetoothPanel");
//    m_toggleBT->setObjectName("bluetoothToggleButton");
//    m_toggleBT->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
//                                          QSizePolicy::Preferred));
    m_cancel->setObjectName("bluetoothCancelButton");
    m_cancel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                        QSizePolicy::Preferred));
    m_modemListDialog = NULL;

//    connect(m_toggleBT, SIGNAL(clicked()), SLOT(toggleBluetoothClicked()));
    connect(m_cancel, SIGNAL(clicked()), SLOT(cancelClicked()));
//    addButton(m_toggleBT->model());
    addButton(m_cancel->model());

    MButton *button1 = new MButton("Connect Phone");
    button1->setObjectName("bluetoothConnectButton");
    button1->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                       QSizePolicy::Preferred));
    connect(button1, SIGNAL(clicked()), this, SLOT(openConnectDialog()));

//    MButton *button2 = new MButton("Other");
//    button2->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
//                                       QSizePolicy::Preferred));
//    button2->setObjectName("bluetoothOtherButton");

    m_layout->setLandscapePolicy(m_land);
    m_land->addItem(button1,  0, 0, Qt::AlignCenter|Qt::AlignVCenter);
//    m_land->addItem(button2,  1, 0, Qt::AlignCenter|Qt::AlignVCenter);

    m_layout->setPortraitPolicy(m_port);
    m_port->addItem(button1,  0, 0, Qt::AlignCenter|Qt::AlignVCenter);
//    m_port->addItem(button2,  1, 0, Qt::AlignCenter|Qt::AlignVCenter);

    setCentralWidget(m_panel);
}

void BluetoothDialog::toggleBluetoothClicked()
{
    TRACE
}

void BluetoothDialog::cancelClicked()
{
    TRACE
}

void BluetoothDialog::openConnectDialog()
{
    ManagerProxy *mp = ManagerProxy::instance();
    if (!mp->isValid() || mp->getModemList().isEmpty())
    {
        qDebug() << QString("No bluetooth modem detected in ofono, phone is probably not paired");
        return;
    }

    if (m_modemListDialog != NULL)
        delete m_modemListDialog;

    m_modemListDialog = new ModemListDialog();
    connect(m_modemListDialog, SIGNAL(modemSelected(QString)), this, SLOT(modemSelected(QString)));
    m_modemListDialog->exec();
}

void BluetoothDialog::hideKeypad()
{
    TRACE
    DialerKeyPad *k = dynamic_cast<MainWindow *>(MApplication::instance()->activeApplicationWindow())->keypad();

    if (!k->isOpen())
        k->open();
    k->disappear();
}

void BluetoothDialog::showKeypad()
{
    TRACE
    MainWindow* mWindow =  dynamic_cast<MainWindow *>(MApplication::instance()->activeApplicationWindow());
    if (mWindow->currentPage() == mWindow->m_pages.at(GenericPage::PAGE_DIALER)) {
        DialerKeyPad *k = mWindow->keypad();
        k->appear();
    }
}

void BluetoothDialog::updateInfo()
{
    TRACE
}

void BluetoothDialog::modemSelected(QString path)
{
    TRACE
    if(m_modemListDialog) {
        m_modemListDialog->done(0);

        /* fixme - delete will cause segfault */
//        delete m_modemListDialog;
//        m_modemListDialog = NULL;
        dismiss();
    }
    emit modemChanged(path);
}

