#
# This file is part of crash-reporter
#
#  Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
#
#  Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
#  Author: Riku Halonen <riku.halonen@nokia.com>
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

TEMPLATE = app
TARGET = crash-reporter-daemon

CONFIG += link_pkgconfig

PKGCONFIG += \
    libudev

packagesExist(qt5-boostable) {
    DEFINES += HAS_BOOSTER
    PKGCONFIG += qt5-boostable
} else {
    warning("qt5-boostable not available; startup times will be slower")
}

INCLUDEPATH += . \
               ../libs/coredir \
               ../libs/httpclient \
               ../libs/utils \
               ../libs/logger \
               ../libs/serviceif \
               ../libs/settings \
               ../libs \
               ../libs/notification \


LIBS += ../../lib/libcrashreporter.so \

QT += dbus network
QT -= gui

SOURCES += main.cpp \
           creporterdaemon.cpp \
           creporterdaemonadaptor.cpp \
           creporterdaemonmonitor.cpp \
           powerexcesshandler.cpp \

HEADERS += creporterdaemon.h \
           creporterdaemon_p.h \
           creporterdaemonadaptor.h \
           creporterdaemonmonitor.h \
           creporterdaemonmonitor_p.h \
           powerexcesshandler.h \

service.files = com.nokia.CrashReporter.Daemon.service
service.path = $$CREPORTER_SYSTEM_DBUS_SERVICES

target.path = $$CREPORTER_SYSTEM_BIN

INSTALLS += target \
            service \
