load(meegotouch_defines)
VERSION = 0.1.13
TARGET = dialer
CONFIG += link_pkgconfig meegotouch debug
PKGCONFIG += meegotouch

# DEFINES += WANT_DEBUG

# Build configuration

!win32-msvc*:QMAKE_CXXFLAGS += -g

# Features
DEFINES += DBUS_SERVICE_PATH=\\\"/com/meego/${QMAKE_TARGET}\\\"
DEFINES += DBUS_SERVICE=\\\"com.meego.${QMAKE_TARGET}\\\"

# FIXME: This should be defined in meegotouch_defines.prf, but is ending up
#        as a NULL or empty value in MeeGo OBS
M_DBUS_SERVICES_DIR = $$M_INSTALL_DATA/dbus-1/services

# Defines for directories, for use in source code.
# They work cross-platform like this.
{
    # FIXME: This is bogus and needs to be removed from here and
    #        callitemview.cpp.  "base" is not garanteed to be the
    #        current theme, ever!
    # THEMEDIR determines the location of the theme
    DEFINES += THEMEDIR=\\\"\"$$M_THEME_DIR/base/meegotouch\"\\\"

    # APPLET_LIBS determines the location where all applet binaries are
    DEFINES += APPLET_LIBS=\\\"\"$$M_APPLET_DIR\"\\\"

    # APPLET_DATA determines where the .desktop files are located
    DEFINES += APPLET_DATA=\\\"\"$$M_APPLET_DATA_DIR\"\\\"

    # APPLET_SETTINGS_DIR determines where the applet global and instance settings files are located
    DEFINES += APPLET_SETTINGS_DIR=\\\"\"$$M_APPLET_SETTINGS_DIR\"\\\"

    # TRANSLATION_DIR determines the default translation path
    DEFINES += TRANSLATION_DIR=\\\"\"$$M_TRANSLATION_DIR\"\\\"

    # M_THEME_PRELOAD_DIR and M_THEME_POST_PRELOAD_DIR defines from where
    # to get lists of images to be preloaded
    DEFINES += M_THEME_PRELOAD_DIR=\\\"\"$$M_THEME_PRELOAD_DIR\"\\\"
    DEFINES += M_THEME_POST_PRELOAD_DIR=\\\"\"$$M_THEME_POST_PRELOAD_DIR\"\\\"
    DEFINES += M_DBUS_SERVICES_DIR=\\\"\"$$M_DBUS_SERVICES_DIR\"\\\"
    DEFINES += M_XDG_DIR=\\\"\"$$M_XDG_DIR\"\\\"
}

# defines for dependencies
!win32:!macx{
    DEFINES += HAVE_CONTEXTSUBSCRIBER
    DEFINES += HAVE_ICU
    DEFINES += HAVE_GCONF
    DEFINES += HAVE_GSTREAMER
}
