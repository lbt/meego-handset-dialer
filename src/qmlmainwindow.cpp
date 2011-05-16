/*
 * dialer - Declarative Dialer UX Main Window.
 * Copyright (c) 2011, Tom Swindell.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "common.h"
#include "dialerapplication.h"

#include "qmlcallitem.h"
#include "qmldialer.h"

#include "qmlmainwindow.h"

#include <QtDeclarative>
#include <MToolBar>

#define CONFIG_KEY_QML_LOAD_URL "/apps/dialer/qml/url"

#define DEFAULT_QML_LOAD_URL "file:///usr/share/meego-handset-dialer/qml/main.qml"

class QMLMainWindowPrivate
{
public:
    QMLMainWindowPrivate()
        : adapter(NULL),
          engine(NULL),
          component(NULL),
          item(NULL),
          mpage(NULL),
          widget(NULL)
    { TRACE }

    QMLDialer               *adapter;
    QDeclarativeEngine      *engine;

    QDeclarativeComponent   *component;
    QDeclarativeItem        *item;

    MApplicationPage        *mpage;
    MWidget                 *widget;
};

static void registerDataTypes()
{
    TRACE
    qmlRegisterType<QMLDialer>("com.meego.dialer", 1, 0, "Dialer");

    qmlRegisterUncreatableType<QMLCallItem>("com.meego.dialer", 1, 0, "CallItem", "");
}

QMLMainWindow::QMLMainWindow(QWidget *parent)
    : MApplicationWindow(parent),
      d(new QMLMainWindowPrivate)
{
    TRACE
    DialerApplication *da = DialerApplication::instance();
    CallManager *cm = ManagerProxy::instance()->callManager();

    this->setOrientationLocked(true);
    this->setPortraitOrientation();
    this->setWindowTitle(qtTrId("xx_window"));

    this->setupUi();

    da->setActiveWindow(this);
}

QMLMainWindow::~QMLMainWindow()
{
    TRACE
    delete this->d;
}

QMLMainWindow* QMLMainWindow::instance()
{
    TRACE
    static QMLMainWindow *_instance = NULL;

    if(_instance == NULL)
    {
        registerDataTypes();
        _instance = new QMLMainWindow;
    }

    return _instance;
}

void QMLMainWindow::setupUi()
{
    TRACE
    MGConfItem qmlUrl(CONFIG_KEY_QML_LOAD_URL);

    d->engine = new QDeclarativeEngine(this);

    d->engine->addImportPath("/usr/share/meego-handset-dialer/qml/base");

    d->engine->rootContext()->setContextProperty("controller", this); //TODO: Remove
    d->engine->rootContext()->setContextProperty("History", DialerApplication::instance()->historyProxy());

    d->component = new QDeclarativeComponent(d->engine, this);
    d->component->loadUrl(qmlUrl.value(DEFAULT_QML_LOAD_URL).toString());

    if(d->component->isError())
    {
        qCritical() << "Failed to load QML Component:" << d->component->errorString();
        return;
    }

    d->item = qobject_cast<QDeclarativeItem*>(d->component->create());
    if(!d->item)
    {
        qCritical() << "Failed to create item from component!";
        return;
    }

    d->mpage = new MApplicationPage;
    d->mpage->setComponentsDisplayMode(MApplicationPage::AllComponents, MApplicationPageModel::Hide);
    d->mpage->setPannable(false);

    d->widget = new MWidget(d->mpage);
    d->item->setSize(d->widget->size());

    QObject::connect(d->widget, SIGNAL(geometryChanged()), SLOT(onGeometryChanged()));

    qobject_cast<QGraphicsItem*>(d->item)->setParentItem(d->widget);

    d->mpage->setCentralWidget(d->widget);
    d->mpage->appear(this);

    this->show();
}

void QMLMainWindow::show()
{
    TRACE
    MApplicationWindow::show();
}

void QMLMainWindow::hide()
{
    TRACE
    MApplicationWindow::hide();
}

void QMLMainWindow::closeEvent(QCloseEvent *event)
{
    TRACE
    if(this->closeOnLazyShutdown())
    {
        this->setCloseOnLazyShutdown(false);
    }
    event->accept();
}

void QMLMainWindow::onGeometryChanged()
{
    TRACE
    d->item->setSize(d->widget->size());
}

void QMLMainWindow::setAdapter(QMLDialer *adapter)
{
    TRACE
    d->adapter = adapter;
}
