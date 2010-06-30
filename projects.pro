include (common.pri)
TEMPLATE = subdirs
SUBDIRS = src

OTHER_FILES += dialer.conf dialer.service

# Icon
desktop_icon.files = ./theme/images/icons-Applications-dialer.png
desktop_icon.path = $$M_INSTALL_DATA/pixmaps
desktop_icon.CONFIG += no_check_exist

# Desktop
desktop_entry.files = dialer.desktop
desktop_entry.path = $$M_INSTALL_DATA/applications
desktop_entry.CONFIG += no_check_exist

# DBus service
dbus_service.files = dialer.service
dbus_service.path = $$M_DBUS_SERVICES_DIR

# Theme files
theme.files = ./theme/*
theme.path = $$M_THEME_DIR/$$TARGET/
theme.CONFIG += no_check_exist

INSTALLS += \
    desktop_icon \
    desktop_entry \
    dbus_service \
    theme
