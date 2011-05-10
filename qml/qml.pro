TEMPLATE = subdirs
CONFIG += ordered

qml.files = meego-handset-dialer/*
qml.path = $${installPrefix}/usr/share/meego-handset-dialer/qml

INSTALLS += qml
