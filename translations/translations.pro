include (../common.pri)
DISABLE_QTTRID_ENGINEERING_ENGLISH=no
LANGUAGES =  # We only create engineering English in the application package
CATALOGNAME = $$TARGET
SOURCEDIR = ../src/
TRANSLATIONDIR = .

# Fix for BMC# 9282
# The following line will disable the prepending of Engineering English
# to all qtTrId wrapped strings by short circuiting the test for
# isEmpty(LRELEASE_OPTIONS) in meegotouch_translations.prf.
LRELEASE_OPTIONS = -idbased

load(meegotouch_translations)
