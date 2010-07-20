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
      m_box(new MStylableWidget()),
      m_layout(new MLayout(m_box)),
      m_policyDefault(new MLinearLayoutPolicy(m_layout, Qt::Vertical)),
      m_policyShowKeypad(new MLinearLayoutPolicy(m_layout, Qt::Vertical)),
      m_policyShowOptions(new MLinearLayoutPolicy(m_layout, Qt::Vertical)),
      m_policyShowBoth(new MLinearLayoutPolicy(m_layout, Qt::Vertical)),
      m_keypadVisible(true),
      m_optionsVisible(true),
      m_incall(false),
      m_optionBox(new MStylableWidget()),
      m_mute(new MButton()),
      m_hold(new MButton()),
      m_spkr(new MButton()),
      m_nway(new MButton()),
      m_buttonBox(new MStylableWidget()),
      m_controlBox(new MStylableWidget()),
      m_add(new MButton()),
      m_call(new MButton()),
      m_hide(new MButton())
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
    m_policyShowBoth->insertItem    (0, m_optionBox,  Qt::AlignCenter);
    m_policyShowBoth->insertItem    (1, m_buttonBox,  Qt::AlignCenter);
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
}

DialerKeyPad::~DialerKeyPad()
{
    TRACE
    delete m_layout;
}

void DialerKeyPad::updateButtonStates()
{
    TRACE
    ManagerProxy  *mp = ManagerProxy::instance();
    CallManager   *cm = (mp)?mp->callManager():0;
    VolumeManager *vm = (mp)?mp->volumeManager():0;
    bool haveCalls = false;

    if (cm && cm->isValid())
        haveCalls = (cm->calls().length() > 0);

    // Sync up the dial/hangup button state
    m_call->setChecked(haveCalls);

    // Sync up the mute button state
    if (vm && vm->isValid())
        m_mute->setChecked(vm->muted());

    // Sync up the mute button state
    if (cm && cm->isValid() && cm->activeCall())
        m_hold->setChecked((cm->activeCall()->state() ==
                            CallItemModel::STATE_HELD));
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
    m_mute->setIconID("icon-m-telephony-ongoing-muted");
    m_mute->setToggledIconID("icon-m-telephony-ongoing-muted-on");
    m_mute->setCheckable(true);
    m_mute->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::MinimumExpanding));

    m_hold->setObjectName("holdButton");
    m_hold->setViewType(MButton::toggleType);
    m_hold->setCheckable(true);
    m_hold->setIconID("icon-dialer-hold");
    m_hold->setToggledIconID("icon-dialer-hold-on");
    m_hold->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::MinimumExpanding));

    m_spkr->setObjectName("speakerButton");
    m_spkr->setViewType(MButton::toggleType);
    m_spkr->setCheckable(true);
    m_spkr->setIconID("icon-dialer-speaker");
    m_spkr->setToggledIconID("icon-dialer-speaker-on");
    m_spkr->setEnabled(false);
    m_spkr->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::MinimumExpanding));

    m_nway->setObjectName("multiButton");
    m_nway->setViewType(MButton::toggleType);
    m_nway->setCheckable(true);
    m_nway->setIconID("icon-m-telephony-call-combine");
    m_nway->setToggledIconID("icon-m-telephony-call-combine-on");
    m_nway->setEnabled(false);
    m_nway->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::MinimumExpanding));

    policy->insertItem(0, m_mute, Qt::AlignHCenter|Qt::AlignBottom);
    policy->insertItem(1, m_hold, Qt::AlignHCenter|Qt::AlignBottom);
    policy->insertItem(2, m_spkr, Qt::AlignHCenter|Qt::AlignBottom);
    policy->insertItem(3, m_nway, Qt::AlignHCenter|Qt::AlignBottom);

    connect(m_mute,  SIGNAL(clicked(bool)), SLOT(mutePressed(bool)));
    connect(m_hold,  SIGNAL(clicked(bool)), SLOT(holdPressed(bool)));
    connect(m_spkr,  SIGNAL(clicked(bool)), SLOT(spkrPressed(bool)));
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
    m_call->setText("Call");
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

void DialerKeyPad::constructNumericKeypad(MGridLayoutPolicy *policy)
{
    TRACE
    MButton *button;
    int k = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            button = new MButton(NumericButtonIcon[k], NumericButtonLabel[k]);
            policy->addItem(button,i,j);
            button->setObjectName("numericButton");
            button->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding));
            connect(button, SIGNAL(clicked(bool)), SLOT(handleButtonClicked()));

            QString label;
	    if (k == 0) // 0 index == "1" key == voicemail
                label = "Call Voicemail";
            else
                label = "Call Speed Dial";
            MAction *callSD = new MAction(label, button);
            callSD->setLocation(MAction::ObjectMenuLocation);
            callSD->setData(k);
            connect(callSD,SIGNAL(triggered()),SLOT(callSpeedDial()));
            button->addAction(callSD);
            if (k != 0) {  // 0 index == "1" key, which is always voicemail
                MAction *setSD = new MAction("Set Speed Dial", button);
                setSD->setLocation(MAction::ObjectMenuLocation);
                connect(setSD,SIGNAL(triggered()),SLOT(setSpeedDial()));
                button->addAction(setSD);
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
    m_call->setText((checked?"End Call":"Call"));
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
        }
    } else {
        CallItem *ac = cm->activeCall();
        if (ac) {
            qDebug() << "Hanging up call to: " << ac->lineID();
            ac->callProxy()->hangup();
        }
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

    // Reset checked state if setMuted() failed
    if (ManagerProxy::instance()->volumeManager()->muted() != checked)
        m_mute->setChecked(ManagerProxy::instance()->volumeManager()->muted());
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
}

void DialerKeyPad::spkrPressed(bool checked)
{
    TRACE
    if (checked)
        qDebug() << "spkr option enabled";
    else
        qDebug() << "spkr option disabled";
}

void DialerKeyPad::nwayPressed(bool checked)
{
    TRACE
    if (checked)
        qDebug() << "nway option enabled";
    else
        qDebug() << "nway option disabled";
}

void DialerKeyPad::callsChanged()
{
    TRACE
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
