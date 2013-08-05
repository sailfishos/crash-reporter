TEMPLATE = lib
TARGET = $$qtLibraryTarget(settingsplugin)

MODULENAME = com/jolla/settings/crashreporter
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

QT += qml
CONFIG += plugin

import.files = qmldir
import.path = $$TARGETPATH

target.path = $$TARGETPATH

SOURCES += plugin.cpp

INSTALLS += target import
