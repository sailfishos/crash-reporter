#
# This file is part of crash-reporter
#
#  Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
#
#  Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
#  Author: Carol Rus <carol.rus@digia.com>
# 
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public License
#  version 2.1 as published by the Free Software Foundation.
# 
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
#  02110-1301 USA
#
#
include(../../crash-reporter-conf.pri)

QT += dbus

CONFIG += meegotouch

TEMPLATE = app
TARGET = crash-reporter-ui

INCLUDEPATH += ../libs/daemonclient \
               ../libs/utils \
               ../libs/dialoginterface \
               ../libs/serviceif \
               ../libs/settings \
               ../dialogplugins \
               ../libs/logger \
               ../libs/notification \
               ../libs/infobanner \
               ../libs \

LIBS += ../../lib/libcrashreporter.so \

SOURCES += main.cpp \
           creporterdialogserver.cpp \
           creporterdialogserverdbusadaptor.cpp \

HEADERS += creporterdialogserver.h \
           creporterdialogserver_p.h \
           creporterdialogserverdbusadaptor.h \

target.path = $$CREPORTER_SYSTEM_BIN

service.files = com.nokia.CrashReporter.DialogServer.service
service.path = $$CREPORTER_SYSTEM_DBUS_SERVICES

INSTALLS += target \
            service \
