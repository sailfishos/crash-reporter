TEMPLATE = lib
TARGET = $$qtLibraryTarget(settingsplugin)

MODULENAME = com/jolla/settings/crashreporter
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

QT += qml
CONFIG += plugin

import.files = qmldir
import.path = $$TARGETPATH

target.path = $$TARGETPATH

INCLUDEPATH += ../../libs \
	../../libs/settings

LIBS += ../../../lib/libcrashreporter.so

SOURCES += plugin.cpp

INSTALLS += target import
