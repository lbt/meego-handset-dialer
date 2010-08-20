load(meegotouch_defines)
TARGET = dialer
TEMPLATE = subdirs
THEME_NAME = $$system(basename $$PWD)

OTHER_FILES += LICENSE                

# Theme files
theme.files = $$system(find ./* -type d)
theme.path = $$M_THEME_DIR/$$THEME_NAME/meegotouch/$$TARGET/
theme.CONFIG += no_check_exist

INSTALLS += theme
