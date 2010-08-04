/*
 * dialer - MeeGo Voice Call Manager
 * Copyright (c) 2009, 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "pacontrol.h"
#include <string.h>
#include <QString>
#include <QList>
#include <QDebug>

// Define our pulse audio loop and connection variables
static PAControl* paControl;

static enum {
    NONE,
    CREATE_LOOPBACKS,
    REMOVE_LOOPBACKS,
    VOLUME_UP,
    VOLUME_DOWN,
    MUTE,
    UNMUTE,
} action = NONE;

static void operation_callback(pa_context *c, int success, void *userdata) {
    if (!success) {
        qDebug() << QString("Operation Failed");
        paControl->setErrorMsg(QString("Operation Failed"));
    }

    pa_threaded_mainloop_signal(paControl->pa_ml, 0);
}

static void module_callback(pa_context *c, uint32_t index, void *userdata) {

    if (index == PA_INVALID_INDEX) {
        qDebug() << QString("Load module failed");
        paControl->setErrorMsg(QString("Load module failed"));
    }

    pa_threaded_mainloop_signal(paControl->pa_ml, 0);
}

static void pa_sourcelist_cb(pa_context *c, const pa_source_info *l, int is_last, void *userdata) {
    PADevice *source;
    int ctr = 0;

    if (is_last > 0) {
        //end of the list
        return;
    }

    source = new PADevice();
    source->index = l->index;
    if(l->name != NULL)
        strncpy(source->name, l->name, 511);
    if(l->description != NULL)
        strncpy(source->description, l->description, 255);
    paControl->sourceList.append(source);

    pa_threaded_mainloop_signal(paControl->pa_ml, 0);
}

static void pa_sinklist_cb(pa_context *c, const pa_sink_info *l, int is_last, void *userdata) {
    PADevice *sink;
    int ctr = 0;

    if (is_last > 0) {
        //end of the list
        return;
    }

    sink = new PADevice();
    sink->index = l->index;
    if(l->name != NULL)
        strncpy(sink->name, l->name, 511);
    if(l->description != NULL)
        strncpy(sink->description, l->description, 255);
    paControl->sinkList.append(sink);

    pa_threaded_mainloop_signal(paControl->pa_ml, 0);
}

static void pa_modulelist_cb(pa_context *c, const pa_module_info *i, int is_last, void *userdata) {
    PAModule *module;
    int ctr = 0;

    if (is_last > 0) {
        //end of the list
        return;
    }

    module = new PAModule();
    module->index = i->index;
    if(i->name != NULL)
        strncpy(module->name, i->name, 511);
    if(i->argument != NULL)
        strncpy(module->argument, i->argument, 255);
    paControl->moduleList.append(module);

    pa_threaded_mainloop_signal(paControl->pa_ml, 0);
}

static void pa_state_cb(pa_context *c, void *userdata) {

    switch (pa_context_get_state(c)) {
        // There are just here for reference
        case PA_CONTEXT_UNCONNECTED:
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
        case PA_CONTEXT_READY:
                pa_threaded_mainloop_signal(paControl->pa_ml, 0);
                break;
        case PA_CONTEXT_TERMINATED:
                break;
        case PA_CONTEXT_FAILED:
                pa_threaded_mainloop_signal(paControl->pa_ml, 0);
                break;
        default:
                break;
    }
}

PAControl::PAControl(QObject *parent)
    :QThread(parent) {
    paControl = this;
    status = SUCCESS;
}

PAControl::~PAControl()
{
    foreach (PADevice *source, sourceList) {
        qDebug() << QString("delete source");
        delete source;
    }
    foreach (PADevice *sink, sinkList) {
        qDebug() << QString("delete sink");
        delete sink;
    }
    foreach (PAModule *module, moduleList) {
        qDebug() << QString("delete module");
        delete module;
    }
    qDebug() << QString("~PAControl()");
}

void PAControl::paInit() {
    // Create a mainloop API and connection to the default server
    pa_ml = pa_threaded_mainloop_new();
    pa_mlapi = pa_threaded_mainloop_get_api(pa_ml);
    pa_ctx = pa_context_new(pa_mlapi, "HFP");

    // This function connects to the pulse server
    pa_context_connect(pa_ctx, NULL, static_cast<pa_context_flags_t>(0), NULL);

    pa_context_set_state_callback(pa_ctx, pa_state_cb, NULL);

    pa_threaded_mainloop_lock(pa_ml);

    // to start the mainloop
    if (pa_threaded_mainloop_start(pa_ml) < 0)
    {
        if (pa_ml)
            pa_threaded_mainloop_unlock(pa_ml);
        paCleanup();
        status = ERROR;
        return;
    }

    /* Wait until the context is ready */
    do
    {
        pa_threaded_mainloop_wait(pa_ml);
    }
    while (pa_context_get_state(pa_ctx) != PA_CONTEXT_READY && pa_context_get_state(pa_ctx) != PA_CONTEXT_FAILED);

    pa_threaded_mainloop_unlock(pa_ml);

    if (pa_context_get_state(pa_ctx) == PA_CONTEXT_FAILED)
    {
        status = ERROR;
    }
}

void PAControl::paCleanup() {
    if(pa_ctx)
        pa_context_disconnect(pa_ctx);
    if(pa_ctx)
        pa_context_unref(pa_ctx);
    if(pa_ml)
        pa_threaded_mainloop_free(pa_ml);
}

PADevice* PAControl::paFindBluezSource() {

    if (sourceList.size() == 0)
    {
        pa_threaded_mainloop_lock(pa_ml);

        pa_op = pa_context_get_source_info_list(pa_ctx, pa_sourcelist_cb, NULL);
        assert(pa_op);

        while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING)
            pa_threaded_mainloop_wait(pa_ml);

        pa_operation_unref(pa_op);
        pa_threaded_mainloop_unlock(pa_ml);
    }

    foreach (PADevice *source, sourceList) {
        QString name;
        name.sprintf("%s", source->name);

        if (name.startsWith(QString("bluez_source."), Qt::CaseSensitive)) {
            qDebug() << QString("   Matched Bluez source: ") << name;
            return source;
        }
    }

    qDebug() << QString("Bluez source: none found");
    return NULL;
}

PADevice*  PAControl::paFindBluezSink() {

    if (sinkList.size() == 0)
    {
        pa_threaded_mainloop_lock(pa_ml);

        pa_op = pa_context_get_sink_info_list(pa_ctx, pa_sinklist_cb, NULL);
        assert(pa_op);

        while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING)
            pa_threaded_mainloop_wait(pa_ml);

        pa_operation_unref(pa_op);
        pa_threaded_mainloop_unlock(pa_ml);
    }

    foreach (PADevice *sink, sinkList) {
        QString name;
        name.sprintf("%s", sink->name);

        if (name.startsWith(QString("bluez_sink."), Qt::CaseSensitive)) {
            qDebug() << QString("   Matched Bluez sink: ") << name;
            return sink;
        }
    }

    qDebug() << QString("Bluez sink: none found");
    return NULL;
}

PADevice* PAControl::paFindAlsaSource() {

    if (sourceList.size() == 0)
    {
        pa_threaded_mainloop_lock(pa_ml);

        pa_op = pa_context_get_source_info_list(pa_ctx, pa_sourcelist_cb, NULL);
        assert(pa_op);

        while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING)
            pa_threaded_mainloop_wait(pa_ml);

        pa_operation_unref(pa_op);
        pa_threaded_mainloop_unlock(pa_ml);
    }

    foreach (PADevice *source, sourceList) {
        qDebug() << QString("Alsa source: ") << source->name;
        QString name;
        name.sprintf("%s", source->name);

        if (name.startsWith(QString("alsa_input."), Qt::CaseSensitive) &&
            name.endsWith(QString("analog-stereo"), Qt::CaseSensitive) &&
            !name.contains(QString("timb"))) {
            qDebug() << QString("   Matched Alsa source: ") << name;
            return source;
        }
    }

    qDebug() << QString("Alsa source: none found");
    return NULL;
}

PADevice*  PAControl::paFindAlsaSink() {

    if (sinkList.size() == 0)
    {
        pa_threaded_mainloop_lock(pa_ml);

        pa_op = pa_context_get_sink_info_list(pa_ctx, pa_sinklist_cb, NULL);
        assert(pa_op);

        while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING)
            pa_threaded_mainloop_wait(pa_ml);

        pa_operation_unref(pa_op);
        pa_threaded_mainloop_unlock(pa_ml);
    }

    foreach (PADevice *sink, sinkList) {
        qDebug() << QString("Alsa sink: ") << sink->name;
        QString name;
        name.sprintf("%s", sink->name);

        if (name.startsWith(QString("alsa_output."), Qt::CaseSensitive) &&
            name.endsWith(QString("analog-stereo"), Qt::CaseSensitive) &&
            !name.contains(QString("timb"))) {
            qDebug() << QString("   Matched Alsa sink: ") << name;
            return sink;
        }
    }

    qDebug() << QString("Alsa sink: none found");
    return NULL;
}

PAModule*  PAControl::paFindLoopbackModule(PADevice* device) {

    if (moduleList.size() == 0)
    {
        pa_threaded_mainloop_lock(pa_ml);

        pa_op = pa_context_get_module_info_list(pa_ctx, pa_modulelist_cb, NULL);
        assert(pa_op);

        while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING)
            pa_threaded_mainloop_wait(pa_ml);

        pa_operation_unref(pa_op);
        pa_threaded_mainloop_unlock(pa_ml);
    }

    foreach (PAModule *module, moduleList) {
        if (strncmp(module->name, "module-loopback", 15)==0) {
            qDebug() << QString("   Matched module: ") << module->name;
            return module;
        }
    }

    qDebug() << QString("Module: none found");
    return NULL;
}

void PAControl::paLoadSourceLoopback(PADevice *source, PADevice *sink) {

    if (source != NULL && sink != NULL) {
        pa_threaded_mainloop_lock(pa_ml);
        QString arg;
        arg.sprintf("source=\"%s\" sink=\"%s\"", source->name, sink->name);

        pa_op = pa_context_load_module(pa_ctx, "module-loopback", arg.toAscii().data(), module_callback, NULL);
        assert(pa_op);
        qDebug() << QString("load-module module-loopback ") << arg;

        while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING)
            pa_threaded_mainloop_wait(pa_ml);

        pa_operation_unref(pa_op);
        pa_threaded_mainloop_unlock(pa_ml);
    }
}

void  PAControl::paLoadSinkLoopback(PADevice *source, PADevice *sink) {

    if (source != NULL && sink != NULL) {
        pa_threaded_mainloop_lock(pa_ml);
        QString arg;
        arg.sprintf("source=\"%s\" sink=\"%s\"", source->name, sink->name);

        pa_op = pa_context_load_module(pa_ctx, "module-loopback", arg.toAscii().data(), module_callback, NULL);
        assert(pa_op);
        qDebug() << QString("load-module module-loopback ") << arg;

        while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING)
            pa_threaded_mainloop_wait(pa_ml);

        pa_operation_unref(pa_op);
        pa_threaded_mainloop_unlock(pa_ml);
    }
}

void PAControl::paToggleMuteSource(PADevice *source, bool isMute) {

    if (source != NULL) {
        pa_threaded_mainloop_lock(pa_ml);

        pa_op = pa_context_set_source_mute_by_name(pa_ctx, source->name, isMute, operation_callback, NULL);
        assert(pa_op);
        qDebug() << QString("set-mute-source ") << source->name;

        while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING)
            pa_threaded_mainloop_wait(pa_ml);

        pa_operation_unref(pa_op);
        pa_threaded_mainloop_unlock(pa_ml);
    }
}

void  PAControl::paUnloadLoopback(PAModule* module) {

    if (module != NULL && module->index >= 0) {
        pa_threaded_mainloop_lock(pa_ml);

        pa_op = pa_context_unload_module(pa_ctx, module->index, operation_callback, NULL);
        assert(pa_op);
        qDebug() << QString("unload-module module-loopback ") << module->index;

        while (pa_operation_get_state(pa_op) == PA_OPERATION_RUNNING)
            pa_threaded_mainloop_wait(pa_ml);

        pa_operation_unref(pa_op);
        pa_threaded_mainloop_unlock(pa_ml);
    }
}

void PAControl::createLoopbacks() {
    action = CREATE_LOOPBACKS;
    this->start();
}

void PAControl::removeLoopbacks() {
    action = REMOVE_LOOPBACKS;
    this->start();;
}

void PAControl::volumeUp() {
    action = VOLUME_UP;
    this->start();
}

void PAControl::volumeDown() {
    action = VOLUME_DOWN;
    this->start();
}

void PAControl::mute() {
    action = MUTE;
    this->start();
}

void PAControl::unmute() {
    action = UNMUTE;
    this->start();
}

PAStatus PAControl::getStatus() {
    return this->status;
}

void PAControl::setErrorMsg(QString msg) {
    if (msg != NULL)
    {
        this->status = ERROR;
        this->errorMsg = msg;
    }
}

QString PAControl::getErrorMsg() {
    return this->errorMsg;
}

void PAControl::run() {

    PADevice* source;
    PADevice* sink;
    PADevice* mic;
    PADevice* speaker;
    int retry = 2;          //retry 2 times, bluez sink source may not show up

    qDebug() << QString("Pulseaudio Client Init()");
    paInit();
    if (status == ERROR) {
        goto cleanup;
    }

    if (action == VOLUME_UP || action == VOLUME_DOWN) {
        qDebug() << QString("Change volume");
        speaker = paFindAlsaSink();
        goto cleanup;
    }

    if (action == MUTE) {
        qDebug() << QString("Mute Mic");
        mic = paFindAlsaSource();
        paToggleMuteSource(mic, true);
        goto cleanup;
    }
    else if (action == UNMUTE) {
        qDebug() << QString("Unmute Mic");
        mic = paFindAlsaSource();
        paToggleMuteSource(mic, false);
        goto cleanup;
    }

    source = paFindBluezSource();
    sink = paFindBluezSink();

    while (((source == NULL) || (sink == NULL)) && retry > 0)
    {
        qDebug() << QString("Bluez source or speaker not found, retry.....") << retry;
        sleep(1);
        source = paFindBluezSource();
        sink = paFindBluezSink();
        retry--;
    }

    if(source == NULL || sink == NULL) {
        qDebug() << QString("Bluez source or speaker not found");
        status = ERROR;
        goto cleanup;
    }

    if(action == CREATE_LOOPBACKS) {

        mic = paFindAlsaSource();
        speaker = paFindAlsaSink();

        if (mic != NULL and speaker != NULL)
        {
            qDebug() << QString("Creating loopback module");
            paLoadSourceLoopback(source, speaker);
            paLoadSinkLoopback(mic, sink);
        }
        else {
            qDebug() << QString("Alsa source and speaker not found");
            status = ERROR;
        }
    }
    else if(action == REMOVE_LOOPBACKS) {
        PAModule* module;

        module = paFindLoopbackModule(source);
        if (module != NULL) {
            qDebug() << QString("Removing loopback module");
            paUnloadLoopback(module);
        }
        else {
            qDebug() << QString("Loopback module not found");
            status = ERROR;
        }
        module = paFindLoopbackModule(sink);
        if (module != NULL) {
            paUnloadLoopback(module);
        }
        else {
            qDebug() << QString("Loopback module not found");
            status = ERROR;
        }
    }

cleanup:
    qDebug() << QString("Pulseaudio Client Cleanup");
    paCleanup();
    qDebug() << QString("Pulseaudio Client Exit");
}


