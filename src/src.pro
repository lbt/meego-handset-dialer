include (../common.pri)
TEMPLATE = app
QT += dbus opengl
CONFIG += qdbus mobility
PKGCONFIG += QtNetwork QtContacts meegobluetooth
MOBILITY += contacts multimedia
MOC_DIR = .moc
OBJECTS_DIR = .obj
MGEN_OUTDIR = .gen
LIBS += -lseaside

target.path = $$M_INSTALL_BIN

STYLE_HEADERS += peopleitemstyle.h \
    callitemstyle.h

MODEL_HEADERS += peopleitemmodel.h \
    callitemmodel.h

SOURCES += main.cpp \
    dialerapplication.cpp \
    mainwindow.cpp \
    dialerkeypad.cpp \
    genericpage.cpp \
    dialerpage.cpp \
    managerproxy.cpp \
    modemproxy.cpp \
    networkproxy.cpp \
    peopleitem.cpp \
    peopleitemview.cpp \
    callproxy.cpp \
    callmanager.cpp \
    callitem.cpp \
    callitemview.cpp \
    callitemmodel.cpp \
    alertdialog.cpp \
    peoplepage.cpp \
    recentpage.cpp \
    favoritespage.cpp \
    debugpage.cpp \
    historytablemodel.cpp \
    dbustypes.cpp \

HEADERS += \
    common.h \
    dialerapplication.h \
    mainwindow.h \
    dialerkeypad.h \
    genericpage.h \
    dialerpage.h \
    managerproxy.h \
    modemproxy.h \
    networkproxy.h \
    peopleitem.h \
    peopleitemview.h \
    callproxy.h \
    callmanager.h \
    callitem.h \
    callitemview.h \
    alertdialog.h \
    peoplepage.h \
    recentpage.h \
    favoritespage.h \
    debugpage.h \
    historytablemodel.h \
    dbustypes.h \
    $$MODEL_HEADERS \
    $$STYLE_HEADERS \
    $$DBUS_INTERFACE_HEADERS \
    $$DBUS_ADAPTOR_HEADERS \

DBUS_ADAPTORS += dbus/com.meego.dialer.xml

DBUS_INTERFACES += \
    dbus/org.ofono.manager.xml \
    dbus/org.ofono.modem.xml \
    dbus/org.ofono.operator.xml \
    dbus/org.ofono.voicecall.xml \

    system(qdbusxml2cpp -i dbustypes.h -p manager_interface.h: dbus/org.ofono.manager.xml)
    system(qdbusxml2cpp -i dbustypes.h -p modem_interface.h: dbus/org.ofono.modem.xml)

MAKE_CLEAN += $$OBJECTS_DIR/*.o
MAKE_DISTCLEAN += \
    $$MOC_DIR/* $$MOC_DIR \
    $$OBJECTS_DIR/* $$OBJECTS_DIR \
    $$MGEN_OUTDIR/* $$MGEN_OUTDIR \

# Install
INSTALLS += target
