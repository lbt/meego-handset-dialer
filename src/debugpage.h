/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef DEBUGPAGE_H
#define DEBUGPAGE_H

#include "genericpage.h"
#include <MLabel>
#include <MContainer>

class MLabel;

class DebugPage : public GenericPage
{
    Q_OBJECT

public:
    DebugPage();
    virtual ~DebugPage();
    virtual void createContent();

public slots:
    void toggleContainerVisible();

private slots:
    void refreshContent();

private:
    MLabel     *mInfo;
    MContainer *mBox;
    MLabel     *vmInfo;
    MContainer *vmBox;
    MLabel     *nInfo;
    MContainer *nBox;
    MLabel     *cInfo;
    MContainer *cBox;
};

#endif // DEBUGPAGE_H
