system(qdbusxml2cpp org.freedesktop.systemd1.Manager.xml -c SystemdManagerProxy -p systemdmanagerproxy.h:systemdmanagerproxy.cpp)
system(qdbusxml2cpp org.freedesktop.systemd1.Unit.xml -c SystemdUnitProxy -p systemdunitproxy.h:systemdunitproxy.cpp)
system(qdbusxml2cpp org.freedesktop.DBus.Properties.xml -c PropertiesProxy -p propertiesproxy.h:propertiesproxy.cpp)

TEMPLATE = lib
TARGET = $$qtLibraryTarget(settingsplugin)

MODULENAME = com/jolla/settings/crashreporter
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

QT += dbus qml
CONFIG += plugin

import.files = qmldir
import.path = $$TARGETPATH

target.path = $$TARGETPATH

INCLUDEPATH += ../../libs \
	../../libs/settings

LIBS += ../../../lib/libcrashreporter.so

QDBUS_GENERATED_HEADERS = \ 
	propertiesproxy.h \
	systemdmanagerproxy.h \
	systemdunitproxy.h

HEADERS += \
	$$QDBUS_GENERATED_HEADERS \
	systemdservice.h \

QDBUS_GENERATED_SOURCES = \
	propertiesproxy.cpp \
	systemdmanagerproxy.cpp \
	systemdunitproxy.cpp

SOURCES += \
	$$QDBUS_GENERATED_SOURCES \
	plugin.cpp \
	systemdservice.cpp

INSTALLS += target import

QMAKE_DISTCLEAN += \
	systemdmanagerproxy.h systemdmanagerproxy.cpp \
	propertiesproxy.h propertiesproxy.cpp \
	systemdunitproxy.h systemdunitproxy.cpp
