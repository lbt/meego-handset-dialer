TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = $$system(find * -type d -prune)
