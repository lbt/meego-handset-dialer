load(meegotouch_defines)
include (../../common.pri)
TEMPLATE = subdirs
THEME_NAME = $$system(basename $$PWD)

OTHER_FILES += dialer.conf

# Icon files
desktop_icon.files = ./icon/icon-Applications-dialer.png
desktop_icon.path = $$M_INSTALL_DATA/pixmaps
desktop_icon.CONFIG += no_check_exist

# Theme files
theme.files = $$system(find ./* -type d)
theme.files += dialer.conf
theme.path = $$M_THEME_DIR/$$THEME_NAME/meegotouch/$$TARGET/
theme.CONFIG += no_check_exist

INSTALLS += \
    desktop_icon \
    theme
