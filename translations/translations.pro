include (../common.pri)
DISABLE_QTTRID_ENGINEERING_ENGLISH=no
LANGUAGES =  # We only create engineering English in the application package
CATALOGNAME = $$TARGET
SOURCEDIR = ../src/
TRANSLATIONDIR = .
load(meegotouch_translations)
