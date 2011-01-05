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

TEMPLATE = lib

QT += dbus

CONFIG += plugin meegotouch

LIBS += -lduicontrolpanel \
              ../../lib/libcrashreporter.so \

INCLUDEPATH += ../libs/settings \
               ../libs/serviceif \
               ../libs/infobanner \
               ../libs \

QMAKE_INCDIR += $${CREPORTER_SYSTEM_INCLUDE}/duicontrolpanel \

SOURCES += creporterprivacysettingswidget.cpp \
           creportersettingsbrief.cpp \
           creportersettingsapplet.cpp \
           creporterprivacydisclaimerdlg.cpp \
           creportermonitorsettingscontainer.cpp \
           creporterincludesettingscontainer.cpp \

HEADERS += creporterprivacysettingswidget.h \
           creportersettings.h \
           creportersettingsbrief.h \
           creportersettingsapplet.h \
           creporterprivacydisclaimerdlg.h \
           creportermonitorsettingscontainer.h \
           creporterincludesettingscontainer.h \

TARGET = $$qtLibraryTarget(crashreportersettingsapplet)

desktop.files = crashreportersettings.desktop
desktop.path = $$[QT_INSTALL_LIBS]/duicontrolpanel

target.path = $$[QT_INSTALL_LIBS]/duicontrolpanel/applets

INSTALLS += target \
            desktop
