#
# This file is part of crash-reporter
#
#  Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
#
#  Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
#  Author: Carol Rus <carol.rus@digia.com>
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

TEMPLATE = lib
CONFIG += dll
QT += network dbus

DEFINES += CREPORTER_EXPORTS

CREPORTER_BUILD_ARCH = $$system(uname -m)
message(Building architecture: $$CREPORTER_BUILD_ARCH)

# Enable Qt Bearer Management API.
!contains(DEFINES, CREPORTER_SDK_HOST) {
    contains(CREPORTER_BUILD_ARCH, armel) {
        message("Building with Qt Bearer Management API support.")
        DEFINES += CREPORTER_LIBBEARER_ENABLED
        SOURCES += httpclient/creporternwsessionmgr.cpp
        HEADERS += httpclient/creporternwsessionmgr.h
    }
}

DESTDIR = ../../lib

!exists(../../lib):system(mkdir -p $$DESTDIR)

INCLUDEPATH += . \
               serviceif \
               utils \
               settings \

DEPENDPATH = $$INCLUDEPATH

SOURCES += httpclient/creporterhttpclient.cpp \
            httpclient/creporteruploaditem.cpp \
            httpclient/creporteruploadqueue.cpp \
            httpclient/creporteruploadengine.cpp \
            utils/creporterutils.cpp \
            logger/creporterlogger.cpp \
            serviceif/creporterdialogserverproxy.cpp \
            serviceif/creporterdaemonproxy.cpp \
            serviceif/creporterautouploaderproxy.cpp \
            settings/creporterprivacysettingsmodel.cpp \
            settings/creportersettingsbase.cpp \
            settings/creportersettingsobserver.cpp \
            settings/creporterapplicationsettings.cpp \
            settings/creportersettingsinit.cpp \
            notification/creporternotification.cpp \

# Public headers
PUBLIC_HEADERS += creporternamespace.h \
                  httpclient/creporterhttpclient.h \
                  httpclient/creporteruploaditem.h \
                  httpclient/creporteruploadqueue.h \
                  httpclient/creporteruploadengine.h \
                  utils/creporterutils.h \
                  dialoginterface/creporterdialogplugininterface.h \
                  dialoginterface/creporterdialogserverinterface.h \
                  logger/creporterlogger.h \
                  serviceif/creporterdaemonproxy.h \
                  serviceif/creporterdialogserverproxy.h \
                  serviceif/creporterautouploaderproxy.h \
                  serviceif/creportermetatypes.h \
                  settings/creporterprivacysettingsmodel.h \
                  settings/creportersettingsbase.h \
                  settings/creportersettingsobserver.h \
                  settings/creporterapplicationsettings.h \
                  notification/creporternotification.h \
                  creporterexport.h \

# Local headers
HEADERS += $$PUBLIC_HEADERS \
            httpclient/creporterhttpclient_p.h \
            httpclient/creporteruploadengine_p.h \
            settings/creportersettingsbase_p.h \
            settings/creportersettingsobserver_p.h \
            settings/creportersettingsinit_p.h \
            notification/creporternotification_p.h \

LIBS += -lssu

TARGET = $$qtLibraryTarget(crashreporter)

target.path += $$[QT_INSTALL_LIBS]

headers.files = $$PUBLIC_HEADERS
headers.path = $$[QT_INSTALL_PREFIX]/include/crash-reporter

# Pkg-config
createpkgconfig.target = $$OUT_PWD/crash-reporter.pc
createpkgconfig.commands = \
    sed -e 's,@VERSION\\@,$$VERSION,g' $$PWD/crash-reporter.pc.in > $$OUT_PWD/crash-reporter.pc.tmp; \
    sed -e 's,@PREFIX\\@,$$[QT_INSTALL_PREFIX],g' $$OUT_PWD/crash-reporter.pc.tmp > $$createpkgconfig.target; \
    rm $$OUT_PWD/crash-reporter.pc.tmp

QMAKE_EXTRA_TARGETS += createpkgconfig

pkgconfig.files = $$OUT_PWD/crash-reporter.pc
pkgconfig.CONFIG += no_check_exist
pkgconfig.path = $$[QT_INSTALL_PREFIX]/lib/pkgconfig
pkgconfig.depends = createpkgconfig

INSTALLS += target \
            pkgconfig \
            headers \
