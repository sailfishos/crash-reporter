#
# This file is part of crash-reporter
#
#  Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
#
#  Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
#  Author: Raimo Gratseff <ext-raimo.gratseff@nokia.com>
#
#  Copyright (C) 2013 Jolla Ltd.
#  Contact: Jakub Adam <jakub.adam@jollamobile.com>
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

TEMPLATE = app
TARGET = crash-reporter-autouploader

INCLUDEPATH += ../libs/serviceif \
               ../libs/settings \
               ../libs/logger \
               ../libs/httpclient \
               ../libs/notification \
               ../libs \

LIBS += ../../lib/libcrashreporter.so \

SOURCES += main.cpp \
           creporterautouploader.cpp \
           creporterautouploaderdbusadaptor.cpp \

HEADERS += creporterautouploader.h \
           creporterautouploaderdbusadaptor.h \

target.path = $$CREPORTER_SYSTEM_BIN

service.files = com.nokia.CrashReporter.AutoUploader.service
service.path = $$CREPORTER_SYSTEM_DBUS_SERVICES

INSTALLS += target \
            service \

