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
#include "dialerkeypad.h"
#include "managerproxy.h"
#include "dialerapplication.h"
#include "mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QGraphicsWidget>
#include <MWidgetView>
#include <MSceneManager>
#include <MAction>
#include <bluedevice.h>
#include <btprofiles.h>
#include <headset.h>

#include <MWidgetCreator>
M_REGISTER_WIDGET(DialerKeyPad)

#define OPTION_INDEX 0
#define BUTTON_INDEX 1
#define CONTROL_INDEX 2
#define BUTTON_ID_KEY 11

enum _NumericButtons {
    _NumericButton_1 = 0,
    _NumericButton_2,
    _NumericButton_3,
    _NumericButton_4,
    _NumericButton_5,
    _NumericButton_6,
    _NumericButton_7,
    _NumericButton_8,
    _NumericButton_9,
    _NumericButton_star,
    _NumericButton_0,
    _NumericButton_hash,
    _NumericButton_MAX,
};

static QString NumericButtonName[_NumericButton_MAX] =
{
  "KeyPad_1",
  "KeyPad_2",
  "KeyPad_3",
  "KeyPad_4",
  "KeyPad_5",
  "KeyPad_6",
  "KeyPad_7",
  "KeyPad_8",
  "KeyPad_9",
  "KeyPad_F",
  "KeyPad_0",
  "KeyPad_P",
};

static QString NumericButtonIcon[_NumericButton_MAX] =
{
  "numk-01-up",
  "numk-02-up",
  "numk-03-up",
  "numk-04-up",
  "numk-05-up",
  "numk-06-up",
  "numk-07-up",
  "numk-08-up",
  "numk-09-up",
  "", //"numk-asterix-icon",
  "numk-00-up",
  "", //"numk-pound-icon",
};

static QString NumericButtonLabel[_NumericButton_MAX] =
{
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "9",
  "*",
  "0",
  "#",
};

static QString NumericButtonMarkup[_NumericButton_MAX] =
{
  "<big><b>1</b></big>",
  "<big><b>2</b></big><br><small>ABC</small>",
  "<big><b>3</b></big><br><small>DEF</small>",
  "<big><b>4</b></big><br><small>GHI</small>",
  "<big><b>5</b></big><br><small>JKL</small>",
  "<big><b>6</b></big><br><small>MNO</small>",
  "<big><b>7</b></big><br><small>PQRS</small>",
  "<big><b>8</b></big><br><small>TUV</small>",
  "<big><b>9</b></big><br><small>WXYZ</small>",
  "<big><b>*</b></big>",
  "<big><b>0</b></big><br><small>+</small>",
  "<big><b>#</b></big>",
};

DialerKeyPad::DialerKeyPad(DialerKeypadType keypadType,
                           MWidget *parent)
    : MOverlay(parent),
      m_type(keypadType),
      m_box(new MStylableWidget(this)),
      m_layout(new MLayout(m_box)),
      m_policyDefault(new MLinearLayoutPolicy(m_layout, Qt::Vertical)),
      m_policyShowKeypad(new MLinearLayoutPolicy(m_layout, Qt::Vertical)),
      m_policyShowOptions(new MLinearLayoutPolicy(m_layout, Qt::Vertical)),
      m_policyShowBoth(new MLinearLayoutPolicy(m_layout, Qt::Vertical)),
      m_keypadVisible(true),
      m_optionsVisible(true),
      m_incall(false),
      m_wirelessConnected(false),
      m_wiredConnected(false),
      m_optionBox(new MStylableWidget()),
      //% "Mute"
      m_mute(new MButton(qtTrId("xx_mute"),this)),
      //% "Hold"
      m_hold(new MButton(qtTrId("xx_hold"),this)),
      //% "Speaker"
      m_audiosink(new MButton(qtTrId("xx_speaker"),this)),
      //% "Merge Calls"
      m_nway(new MButton(qtTrId("xx_merge"),this)),
      m_buttonBox(new MStylableWidget()),
      m_controlBox(new MStylableWidget()),
      m_add(new MButton(this)),
      m_call(new MButton(this)),
      m_hide(new MButton(this)),
      bluetoothDevices(new DeviceModel(this))
{
    TRACE
    m_box->setParent(this);

    m_layout->setPolicy(m_policyDefault);
    m_policyDefault->setSpacing(0);
    m_policyDefault->setContentsMargins(0,0,0,0);
    m_policyShowKeypad->setSpacing(0);
    m_policyShowKeypad->setContentsMargins(0,0,0,0);
    m_policyShowOptions->setSpacing(0);
    m_policyShowOptions->setContentsMargins(0,0,0,0);
    m_policyShowBoth->setSpacing(0);
    m_policyShowBoth->setContentsMargins(0,0,0,0);

    m_box->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                     QSizePolicy::MinimumExpanding));

    createOptionBox();
    createButtonBox();
    createControlBox();

    // Default policy shows only the call controls
    m_policyDefault->insertItem     (0, m_controlBox, Qt::AlignCenter);

    // ShowKeypad policy shows only the keypad and call controls
    m_policyShowKeypad->insertItem  (0, m_buttonBox,  Qt::AlignCenter);
    m_policyShowKeypad->insertItem  (1, m_controlBox, Qt::AlignCenter);

    // ShowOptions policy shows only the call options and call controls
    m_policyShowOptions->insertItem (0, m_optionBox,  Qt::AlignCenter);
    m_policyShowOptions->insertItem (1, m_controlBox, Qt::AlignCenter);

    // ShowBoth policy shows call options, keypad and call controls
    m_policyShowBoth->insertItem    (0, m_buttonBox,  Qt::AlignCenter);
    m_policyShowBoth->insertItem    (1, m_optionBox,  Qt::AlignCenter);
    m_policyShowBoth->insertItem    (2, m_controlBox, Qt::AlignCenter);

    setObjectName("keypadOverlay");
    m_box->setObjectName("keypadOverlayBox");
    m_policyDefault->setObjectName("keypadOverlayLayout");
    m_optionBox->setObjectName("keypadOptionBox");
    m_buttonBox->setObjectName("keypadButtonBox");
    m_controlBox->setObjectName("keypadControlBox");

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                              QSizePolicy::MinimumExpanding));

    setWidget(m_box);

    ManagerProxy *mp = ManagerProxy::instance();
    if (mp && mp->callManager() && mp->callManager()->isValid()) {
        m_call->setChecked((mp->callManager()->calls().length() > 0));
        connect (mp->callManager(), SIGNAL(callsChanged()),
                                    SLOT(callsChanged()));
    } else {
        qWarning() << QString("No valid CallManager instance available, will not capture call state changes");
    }

    connect(this, SIGNAL(appeared()), SLOT(updateLayoutPolicy()));

    foreach(QDBusObjectPath path, bluetoothDevices->devices()) {
        bluetoothDeviceCreated(path);
    }

    connect(bluetoothDevices, SIGNAL(deviceCreated(QDBusObjectPath)), this, SLOT(bluetoothDeviceCreated(QDBusObjectPath)));
    connect(bluetoothDevices, SIGNAL(deviceRemoved(QDBusObjectPath)), this, SLOT(bluetoothDeviceRemoved(QDBusObjectPath)));
}

DialerKeyPad::~DialerKeyPad()
{
    TRACE
}

void DialerKeyPad::updateButtonStates()
{
    TRACE
    ManagerProxy  *mp = ManagerProxy::instance();
    CallManager   *cm = (mp)?mp->callManager():0;
    VolumeManager *vm = (mp)?mp->volumeManager():0;
    bool haveCalls = false;

    if (cm && cm->isValid())
        haveCalls = ((cm->calls().length() > 0) ||
                     (cm->multipartyCalls()));

    // Sync up the dial/hangup button state
    m_call->setChecked(haveCalls);

    // Sync up the mute button state
    if (vm && vm->isValid()) {
        m_mute->setChecked(vm->muted());
        if (vm->muted())
            //% "Un-mute"
            m_mute->setText(qtTrId("xx_un_mute"));
        else
            //% "Mute"
            m_mute->setText(qtTrId("xx_mute"));
    }

    // Sync up the hold button state
    if (cm && cm->isValid()) {
        m_hold->setEnabled(true); // Start by re-enabling the button
        if (cm->activeCall() && cm->heldCall())
            //% "Swap"
            m_hold->setText(qtTrId("xx_swap"));
        else if (cm->activeCall()) {
            //% "Hold"
            m_hold->setText(qtTrId("xx_hold"));
            m_hold->setChecked(false);
        }
        else if (cm->heldCall()) {
            //% "Un-Hold"
            m_hold->setText(qtTrId("xx_un_hold"));
            m_hold->setChecked(true);
        }
        else {
            m_hold->setEnabled(false);
            m_hold->setChecked(false);
        }
    }

    // Sync up the merge button state
    if (cm && cm->isValid()) {
        if (cm->multipartyCalls())
            //% "Add"
            m_nway->setText(qtTrId("xx_add"));
        else
            //% "Merge Calls"
            m_nway->setText(qtTrId("xx_merge"));
    }

    /*
       When audiosink button is checked, we must set the PA sink to use
       the "alternate" output device, while setting the button label
       to indicate the name of the "default" device.

       When audiosink button is unchecked, we must set the PA sink to use
       the "default" output device, while setting the button label
       to indicate the name of the "alternate" device.

       All of this depends on what output devices are actually connected...

       IOW: The button label should always indicate what *will* be set as
            the output device sink if the button is pressed, NOT what *is*
            the current output device sink

       Audio output path order of precedence:
       1. Wired
       2. Wireless (BT HSP)
       3. Handset
       4. Speaker

       So when the audiosink button is pressed, the alternate output
       depends on what is connected at the time

       if (audiosink checked)
           if (Wired connected)
               if (Wireless connected)
                   Set output path to Wireless
               else
                   Set output path to Handset
           else if (Wireless connected)
               Set output path to Handset
           else
               Set output path to Speaker
       else
           if (Wired connected)
               Set output path to Wired
           else if (Wireless connected)
               Set output path to Wireless
           else
               Set output path to Handset
     */
    if (m_audiosink->isChecked()) {
        if (m_wiredConnected) {
            if (m_wirelessConnected) {
                //% "Bluetooth Headset"
                m_audiosink->setText(qtTrId("xx_bluetooth_headset"));
            }
            else {
                //% "Handset"
                m_audiosink->setText(qtTrId("xx_handset"));
            }
            // TODO: Tell PA to set sink to Wired Headset sink device
        }
        else if (m_wirelessConnected) {
            //% "Handset"
            m_audiosink->setText(qtTrId("xx_handset"));
            // TODO: Tell PA to set sink to Wireless Headset sink device
        }
        else {
            //% "Handset"
            m_audiosink->setText(qtTrId("xx_handset"));
            // TODO: Tell PA to set sink to Speaker sink device
        }
    }
    else { // unchecked
        if (m_wiredConnected) {
            //% "Wired Headset"
            m_audiosink->setText(qtTrId("xx_wired_headset"));
            // TODO: Tell PA to set sink to Handset sink device
        }
        else if (m_wirelessConnected) {
            //% "Bluetooth Headset"
            m_audiosink->setText(qtTrId("xx_bluetooth_headset"));
            // TODO: Tell PA to set sink to Handset sink device
        }
        else {
            //% "Speaker"
            m_audiosink->setText(qtTrId("xx_speaker"));
            // TODO: Tell PA to set sink to Handset sink device
        }
    }
}

void DialerKeyPad::updateLayoutPolicy()
{
    TRACE

    ManagerProxy *mp = ManagerProxy::instance();
    CallManager  *cm = (mp)?mp->callManager():0;
    bool haveCalls = false;
    MLinearLayoutPolicy *newPolicy;

    if (cm && cm->isValid())
        haveCalls = (cm->calls().length() > 0);

    newPolicy = m_policyDefault;
    if (haveCalls && isOpen())
        newPolicy = m_policyShowBoth;
    else if (haveCalls)
        newPolicy = m_policyShowOptions;
    else if (isOpen())
        newPolicy = m_policyShowKeypad;
    else
        newPolicy = m_policyDefault;

    // Only update the policy if it needs to be changed
    if (newPolicy != m_layout->policy())
        m_layout->setPolicy(newPolicy);

    // Now adjust keypad position as needed
    QSizeF ks = m_box->sizeHint(Qt::PreferredSize);
    QSizeF ss = sceneManager()->visibleSceneSize();

    if (isVisible())
        setPos(QPointF(0,ss.rheight()) + QPointF(0, -(ks.rheight())));
    else
        setPos(QPointF(0,ss.rheight()) + QPointF(0, +(ks.rheight())));

    // Finally, sync up the button states
    updateButtonStates();
}

void DialerKeyPad::setTarget(MTextEdit *target)
{
    TRACE
    m_target = target;
}

MTextEdit *DialerKeyPad::target()
{
    TRACE
    return m_target;
}

bool DialerKeyPad::isOpen()
{
    TRACE
    return m_keypadVisible;
}

void DialerKeyPad::open()
{
    TRACE
    setKeypadVisible(true);
}

void DialerKeyPad::close()
{
    TRACE
    setKeypadVisible(false);
}

void DialerKeyPad::createOptionBox()
{
    TRACE
    MLayout *layout = new MLayout(m_optionBox);
    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout,
                                                              Qt::Horizontal);
    policy->setObjectName("keypadOptionLayout");

    m_mute->setObjectName("muteButton");
    m_mute->setViewType(MButton::toggleType);
//    m_mute->setIconID("icon-m-telephony-ongoing-muted");
//    m_mute->setToggledIconID("icon-m-telephony-ongoing-muted-on");
    m_mute->setCheckable(true);
    m_mute->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::MinimumExpanding));

    m_hold->setObjectName("holdButton");
    m_hold->setViewType(MButton::toggleType);
    m_hold->setCheckable(true);
//    m_hold->setIconID("icon-dialer-hold");
//    m_hold->setToggledIconID("icon-dialer-hold-on");
    m_hold->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::MinimumExpanding));

    m_audiosink->setObjectName("speakerButton");
    m_audiosink->setViewType(MButton::toggleType);
    m_audiosink->setCheckable(true);
//    m_spkr->setIconID("icon-dialer-speaker");
//    m_spkr->setToggledIconID("icon-dialer-speaker-on");
    m_audiosink->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::MinimumExpanding));

    m_nway->setObjectName("multiButton");
    m_nway->setViewType(MButton::toggleType);
    m_nway->setCheckable(true);
//    m_nway->setIconID("icon-m-telephony-call-combine");
//    m_nway->setToggledIconID("icon-m-telephony-call-combine-on");
    m_nway->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::MinimumExpanding));

    policy->insertItem(0, m_hold, Qt::AlignHCenter|Qt::AlignBottom);
    policy->insertItem(1, m_mute, Qt::AlignHCenter|Qt::AlignBottom);
    policy->insertItem(2, m_audiosink, Qt::AlignHCenter|Qt::AlignBottom);
    policy->insertItem(3, m_nway, Qt::AlignHCenter|Qt::AlignBottom);

    connect(m_mute,  SIGNAL(clicked(bool)), SLOT(mutePressed(bool)));
    connect(m_hold,  SIGNAL(clicked(bool)), SLOT(holdPressed(bool)));
    connect(m_audiosink,  SIGNAL(clicked(bool)), SLOT(audiosinkPressed(bool)));
    connect(m_nway,  SIGNAL(clicked(bool)), SLOT(nwayPressed(bool)));
}

void DialerKeyPad::createButtonBox()
{
    TRACE
    MLayout *layout = new MLayout(m_buttonBox);
    MGridLayoutPolicy *policy = new MGridLayoutPolicy(layout);
    policy->setObjectName("keypadButtonLayout");

    policy->setSpacing(0);
    policy->setContentsMargins(0,0,0,0);

    switch (m_type) {
        case DialerKeypadQwerty:
            constructQwertyKeypad();
            break;
        case DialerKeypadNumeric:
        default:
            constructNumericKeypad(policy);
            break;
    }
}

void DialerKeyPad::createControlBox()
{
    TRACE
    MLayout *layout = new MLayout(m_controlBox);
    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout,
                                                              Qt::Horizontal);
    policy->setObjectName("keypadControlLayout");

    m_add->setObjectName("addButton");
    m_add->setIconID("icon-m-common-add-contact");
    m_add->setEnabled(false);
    m_add->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                     QSizePolicy::MinimumExpanding));

    m_call->setObjectName("callButton");
    m_call->setViewType(MButton::toggleType);
    //% "Call"
    m_call->setText(qtTrId("xx_call"));
    m_call->setCheckable(true);
    m_call->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::MinimumExpanding));

    m_hide->setObjectName("hideButton");
    m_hide->setViewType(MButton::toggleType);
    m_hide->setCheckable(true);
    m_hide->setChecked(true);
    m_hide->setIconID("icon-dialpad-open");
    m_hide->setToggledIconID("icon-dialpad-close");
    m_hide->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::MinimumExpanding));

    policy->insertItem(0, m_add, Qt::AlignHCenter|Qt::AlignBottom);
    policy->insertItem(1, m_call, Qt::AlignHCenter|Qt::AlignBottom);
    policy->insertItem(2, m_hide, Qt::AlignHCenter|Qt::AlignBottom);

    connect(m_add,  SIGNAL(clicked(bool)), SLOT(addPressed()));
    connect(m_call, SIGNAL(clicked(bool)), SLOT(callPressed(bool)));
    connect(m_call, SIGNAL(toggled(bool)), SLOT(callToggled(bool)));
    connect(m_hide, SIGNAL(clicked(bool)), SLOT(setKeypadVisible(bool)));
}

void DialerKeyPad::setKeypadVisible(bool visible)
{
    TRACE
    m_keypadVisible = visible;

    updateLayoutPolicy();

    emit (isOpen())?opened():closed();

    // Sync up show/hide button (Checked == show, it's icon will be "hide")
    if (m_hide->isChecked() != isOpen())
        m_hide->setChecked(isOpen());
}

void DialerKeyPad::forceKeypadVisible()
{
    TRACE
    m_buttonBox->setVisible(true);
}

void DialerKeyPad::handleButtonClicked()
{
    TRACE
    MButton * button = dynamic_cast<MButton *>(sender());

    qDebug() << "Button pressed: " << button->text();

    if (m_target && button && !button->text().isEmpty()) {
        m_target->insert(button->text());
    }

    CallManager *cm = ManagerProxy::instance()->callManager();
    if (!cm->isValid()) {
        qDebug() << "Unable to determine if in active call, no valid connection";
        return;
    }
    if (cm->activeCall()) {
      cm->sendTones(button->text());
    }
}

void DialerKeyPad::callSpeedDial()
{
    TRACE
    qDebug() << "Call Speed Dial invoked";

    int id = dynamic_cast<MAction *>(sender())->data().toInt();

    if (id == 0) { // 0 index == "1" key == voicemail
        ManagerProxy *mp = ManagerProxy::instance();
        if ((mp->voicemail() != 0) && (mp->voicemail()->isValid()) &&
             m_target && !mp->voicemail()->mailbox().isEmpty()) {
            m_target->setText(mp->voicemail()->mailbox());
        }
    }
    else {
        SHOW_TBD
    }
}

void DialerKeyPad::setSpeedDial()
{
    TRACE
    qDebug() << "Set Speed Dial requested";
    SHOW_TBD
}

void DialerKeyPad::bluetoothDeviceCreated(QDBusObjectPath path)
{
    OrgBluezDeviceInterface device("org.bluez",path.path(),QDBusConnection::systemBus());
    QVariantMap properties = device.GetProperties();

    QStringList uuidlist = properties["UUIDs"].toStringList();

    foreach(QString uuid, uuidlist) {
        if(uuid.toLower() == BluetoothProfiles::hs || uuid.toLower() == BluetoothProfiles::hf) {
            OrgBluezHeadsetInterface *headset = new OrgBluezHeadsetInterface("org.bluez",
                                                                             path.path(),
                                                                             QDBusConnection::systemBus(),
                                                                             this);
            if(headset->IsConnected()) {
                headsetConnected();
            }
            connect(headset,SIGNAL(Connected()),this,SLOT(headsetConnected()));
            connect(headset,SIGNAL(Disconnected()),this,SLOT(headsetDisconnected()));
            break;
        }
    }
}

void DialerKeyPad::bluetoothDeviceRemoved(QDBusObjectPath)
{

}

void DialerKeyPad::headsetConnected()
{
    m_wirelessConnected=true;
    updateButtonStates();
}

void DialerKeyPad::headsetDisconnected()
{
    m_wirelessConnected=false;
    updateButtonStates();
}

void DialerKeyPad::constructNumericKeypad(MGridLayoutPolicy *policy)
{
    TRACE
    MButton *button;
    int k = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            button = new MButton(NumericButtonIcon[k], NumericButtonLabel[k]);
            policy->addItem(button,i,j);
            button->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding));
            connect(button, SIGNAL(clicked(bool)), SLOT(handleButtonClicked()));

            switch (k) {
            case _NumericButton_star:
                button->setObjectName("starButton");
                break;
            case _NumericButton_hash:
                button->setObjectName("hashButton");
                break;
            default:
                button->setObjectName("numericButton");
                if (k == _NumericButton_0)  // "0" button never calls speed dial
                    break;

                // Add the "call speed dial" action
                //% "Call Speed Dial"
                QString label = qtTrId("xx_call_speeddial");
                if (k == _NumericButton_1)
                    //% "Call Voicemail"
                    label = qtTrId("xx_call_voicemail");

                MAction *callSD = new MAction(label, button);
                callSD->setLocation(MAction::ObjectMenuLocation);
                callSD->setData(k);
                connect(callSD,SIGNAL(triggered()),SLOT(callSpeedDial()));
                button->addAction(callSD);

                // Add the "set speed dial" action, except for "1" key
                if (k != _NumericButton_1) {
                    //% "Set Speed Dial"
                    QString label = qtTrId("xx_set_speeddial");
                    MAction *setSD = new MAction(label,button);
                    setSD->setLocation(MAction::ObjectMenuLocation);
                    connect(setSD,SIGNAL(triggered()),SLOT(setSpeedDial()));
                    button->addAction(setSD);
                }
                break;
            }
            k++;
            m_buttons << button;
        }
    }
}

void DialerKeyPad::constructQwertyKeypad()
{
    TRACE
}

void DialerKeyPad::callToggled(bool checked)
{
    TRACE
    if (checked)
        //% "End Call"
        m_call->setText(qtTrId("xx_end_call"));
    else
        //% "Call"
        m_call->setText(qtTrId("xx_call"));
}

void DialerKeyPad::callPressed(bool checked)
{
    TRACE
    CallManager *cm = ManagerProxy::instance()->callManager();
    if (!cm->isValid()) {
        qDebug() << "Unable to dial, no valid connection";
        return;
    }

    if (checked) {
        if (m_target && !m_target->text().isEmpty()) {
            QString number = stripLineID(m_target->text());
            qDebug() << "Placing call to: " << number;
            cm->dial(number);
            this->setKeypadVisible(false); // BMC# 6809 - NW
        }
        else
            // No number to dial, set back to unchecked, Fixes BMC#3284
            m_call->setChecked(false);
    }
    else {
        CallItem *c = NULL;
        if (cm->activeCall())
            c = cm->activeCall();
        else if (cm->heldCall())
            c = cm->heldCall();
        else if (cm->dialingCall()) // Fixes BMC#432
            c = cm->dialingCall();
        else if (cm->incomingCall()) // Fixes BMC#7536
            c = cm->incomingCall();
        else if (cm->waitingCall()) // Fixes BMC#7536
            c = cm->waitingCall();
        else if (cm->alertingCall()) // Fixes BMC#8322
            c = cm->alertingCall();

        if (c) {
            if (cm->multipartyCalls()) {
                qDebug() << "Hanging up MultipartyCall";
                cm->hangupMultipartyCall();
            }
            else {
                qDebug() << "Hanging up call to: " << c->lineID();
                c->callProxy()->hangup();
            }
        }
        else
            qWarning() << "Hangup requested when no active or held calls!";
    }
}

void DialerKeyPad::addPressed()
{
    TRACE
    qDebug() << "add pressed";
}

// TODO: Eventually need to complete the muteChanged signal implementation
//       in CallVolume class and connect a local slot to it so we can
//       maintain state with ofono
void DialerKeyPad::mutePressed(bool checked)
{
    TRACE
    CallManager *cm = ManagerProxy::instance()->callManager();
    if (!cm->isValid()) {
        qDebug() << "Unable to mute, no valid connection";
        return;
    }
    if (!cm->activeCall()) {
        qDebug() << "Unable to mute, no active call";
        return;
    }

    qDebug() << QString("mute option %1").arg((checked)?"set":"unset");
    ManagerProxy::instance()->volumeManager()->setMuted(checked);

    // Sync up the button states
    updateButtonStates();
}

void DialerKeyPad::holdPressed(bool checked)
{
    TRACE
    CallManager *cm = ManagerProxy::instance()->callManager();
    if (!cm->isValid()) {
        qDebug() << "Unable to hold, no valid connection";
        return;
    }

    qDebug() << QString("hold option %1").arg((checked)?"set":"unset");
    cm->swapCalls();

    // Sync up the button states
    updateButtonStates();
}

void DialerKeyPad::audiosinkPressed(bool checked)
{
    TRACE
    qDebug() << QString("audiosink option %1").arg((checked)?"set":"unset");

    // Sync up the button states
    updateButtonStates();
}

void DialerKeyPad::nwayPressed(bool checked)
{
    TRACE
    CallManager *cm = ManagerProxy::instance()->callManager();
    if (!cm->isValid()) {
        qDebug() << "Unable to merge, no valid connection";
        return;
    }

    qDebug() << QString("nway option %1").arg((checked)?"set":"unset");

    // If there is already a MultipartyCall, then we want to add an new
    // participant
    if (cm->multipartyCalls())
        if (cm->activeCall() && cm->heldCall())
            qCritical() << QString("Can't add participant all lines busy!");
        else
            qWarning() << QString("Add to MultipartyCall not yet working...");
    // Otherwise, we're merging existing calls into a MultipartyCall
    // Fixes BMC#550, and BMC#2806
    else
        cm->createMultipartyCall();

    // Sync up the button states
    updateButtonStates();
}

void DialerKeyPad::callsChanged()
{
    TRACE

    if (isOnDisplay())
        updateLayoutPolicy();
/*
    ManagerProxy *mp = ManagerProxy::instance();
    if (mp && mp->callManager() && mp->callManager()->isValid()) {
        m_call->setChecked((mp->callManager()->calls().length() > 0));
    } else {
        qWarning() << QString("No valid CallManager instance available, could not set call button state correctly");
    }
*/
}
