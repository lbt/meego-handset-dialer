/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PACONTROL_H
#define PACONTROL_H
#include <pulse/context.h>
#include <pulse/pulseaudio.h>
#include <QThread>

typedef struct paDevice {
    char name[512];
    int index;
    char description[512];
} PADevice;

typedef struct paModule {
    char name[512];
    int index;
    char argument[512];
} PAModule;

enum PAStatus {
    SUCCESS = 0,
    ERROR = 1,
};

class PAControl : public QThread
{
    Q_OBJECT
public:
    pa_threaded_mainloop *pa_ml;
    QList<PADevice*> sourceList;
    QList<PADevice*> sinkList;
    QList<PAModule*> moduleList;

    PAControl(QObject *parent = 0);
    ~PAControl();

    void createLoopbacks();
    void removeLoopbacks();
    void volumeUp();
    void volumeDown();
    void mute();
    void unmute();
    PAStatus getStatus();
    void setErrorMsg(QString msg);
    QString getErrorMsg();

private:
    pa_mainloop_api *pa_mlapi;
    pa_operation *pa_op;
    pa_context *pa_ctx;
    PAStatus status;
    QString errorMsg;

    void paInit();
    void paCleanup();
    PADevice* paFindBluezSource();
    PADevice* paFindBluezSink();
    PADevice* paFindAlsaSource();
    PADevice* paFindAlsaSink();
    PAModule* paFindLoopbackModule(PADevice *device);
    void paLoadSourceLoopback(PADevice *source, PADevice *sink);
    void paLoadSinkLoopback(PADevice *source, PADevice *sink);
    void paUnloadLoopback(PAModule* module);
    void paToggleMuteSource(PADevice *source, bool isMute);

protected:
    void run();
};

#endif // PACONTROL_H
