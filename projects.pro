include (common.pri)
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = src qml themes translations

#OTHER_FILES += dialer.service
OTHER_FILES += *.service *.desktop *.sh

# Keepalive script
keepalive_script.files = dialer-keepalive.sh
keepalive_script.path = $$M_INSTALL_BIN
keepalive_script.CONFIG += no_check_exist

# XDG Autostart
#autostart_entry.files = dialer.desktop
autostart_entry.files = dialer-prestart.desktop
autostart_entry.path = $$M_XDG_DIR/autostart
autostart_entry.CONFIG += no_check_exist

# Desktop
desktop_entry.files = dialer.desktop
desktop_entry.path = $$M_INSTALL_DATA/applications
desktop_entry.CONFIG += no_check_exist

# DBus service
dbus_service.files = dialer.service
dbus_service.path = $$M_DBUS_SERVICES_DIR

INSTALLS += \
    keepalive_script \
    autostart_entry \
    desktop_entry \
    dbus_service
