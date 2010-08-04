/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef DIALERKEYPAD_H
#define DIALERKEYPAD_H

#include <QObject>
#include <MOverlay>
#include <MTextEdit>
#include <MLayout>
#include <MGridLayoutPolicy>
#include <MLinearLayoutPolicy>
#include <MButton>
#include <MContainer>
#include <MStylableWidget>
#ifdef IVI_HFP
#include <QTimer>
#endif

typedef enum _DialerKeypadType {
    DialerKeypadNumeric,
    DialerKeypadQwerty,
} DialerKeypadType;

class DialerKeyPad : public MOverlay
{
    Q_OBJECT;

public:
    DialerKeyPad(DialerKeypadType keypadType = DialerKeypadNumeric,
                 MWidget *parent = 0);
    virtual ~DialerKeyPad();

    void setTarget(MTextEdit *text);

    MTextEdit *target();
    bool         isOpen();

public Q_SLOTS:
    void open();
    void close();

signals:
    void closed();
    void opened();

private:
    DialerKeypadType       m_type;
    MStylableWidget     *m_box;
    MLayout             *m_layout;
    MLinearLayoutPolicy *m_policyDefault;
    MLinearLayoutPolicy *m_policyShowKeypad;
    MLinearLayoutPolicy *m_policyShowOptions;
    MLinearLayoutPolicy *m_policyShowBoth;
    MTextEdit           *m_target;
    bool                   m_keypadVisible;
    bool                   m_optionsVisible;
    bool                   m_incall;

    MStylableWidget     *m_optionBox;
    MButton             *m_mute, *m_hold, *m_spkr, *m_nway;
    MStylableWidget     *m_buttonBox;
    QList<MButton*>      m_buttons;
    MStylableWidget     *m_controlBox;
    MButton             *m_add, *m_call, *m_hide;
#ifdef IVI_HFP
    MButton             *m_bksp;
    bool                 m_pressed;
    QTimer               m_tapnhold;
#endif

    void constructNumericKeypad(MGridLayoutPolicy*);
    void constructQwertyKeypad();
    void createOptionBox();
    void createButtonBox();
    void createControlBox();

private Q_SLOTS:
    void updateButtonStates();
    void updateLayoutPolicy();
    void forceKeypadVisible();
    void handleButtonClicked();
    void setKeypadVisible(bool);
    void callToggled(bool);
    void callPressed(bool);
    void addPressed();
    void mutePressed(bool);
    void holdPressed(bool);
    void spkrPressed(bool);
    void nwayPressed(bool);
    void callsChanged();
    void callSpeedDial();
    void setSpeedDial();
#ifdef IVI_HFP
    void doClear();
    void doBackspace();
    void handleBkspPress();
    void handleBkspRelease();
#endif
};

#endif // DIALERKEYPAD_H
