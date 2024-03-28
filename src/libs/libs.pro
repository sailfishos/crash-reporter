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
QT = \
    core \
    network \
    dbus

CONFIG += link_pkgconfig
PKGCONFIG += mce \
             usb-moded-qt5 \
             nemonotifications-qt5 \
             systemsettings

DEFINES += CREPORTER_EXPORTS

message(Building architecture: $$system(uname -m))

# Enable Qt Bearer Management API.
!contains(DEFINES, CREPORTER_SDK_HOST) {
    message("Building with Qt Bearer Management API support.")
    DEFINES += CREPORTER_LIBBEARER_ENABLED
    SOURCES += httpclient/creporternwsessionmgr.cpp
    HEADERS += httpclient/creporternwsessionmgr.h
}

DESTDIR = ../../lib

!exists(../../lib):system(mkdir -p $$DESTDIR)

INCLUDEPATH += . \
               coredir \
               serviceif \
               utils \
               settings \

DEPENDPATH = $$INCLUDEPATH

DBUS_INTERFACES = \
	utils/org.nemo.ssu.xml \
	../autouploader/com.nokia.CrashReporter.AutoUploader.xml \

SOURCES += coredir/creportercoredir.cpp \
           coredir/creportercoreregistry.cpp \
           httpclient/creporterhttpclient.cpp \
           httpclient/creporteruploaditem.cpp \
           httpclient/creporteruploadqueue.cpp \
           httpclient/creporteruploadengine.cpp \
           utils/creporterutils.cpp \
           logger/creporterlogger.cpp \
           serviceif/creporterdaemonproxy.cpp \
           settings/creporterprivacysettingsmodel.cpp \
           settings/creportersavedstate.cpp \
           settings/creportersettingsbase.cpp \
           settings/creporterapplicationsettings.cpp \
           settings/creportersettingsinit.cpp \

# Public headers
PUBLIC_HEADERS += creporternamespace.h \
                  coredir/creportercoredir.h \
                  coredir/creportercoreregistry.h \
                  httpclient/creporterhttpclient.h \
                  httpclient/creporteruploaditem.h \
                  httpclient/creporteruploadqueue.h \
                  httpclient/creporteruploadengine.h \
                  utils/creporterutils.h \
                  logger/creporterlogger.h \
                  serviceif/creporterdaemonproxy.h \
                  serviceif/creportermetatypes.h \
                  settings/creporterprivacysettingsmodel.h \
                  settings/creportersavedstate.h \
                  settings/creportersettingsbase.h \
                  settings/creporterapplicationsettings.h \
                  creporterexport.h \

# Local headers
HEADERS += $$PUBLIC_HEADERS \
           coredir/creportercoredir_p.h \
           coredir/creportercoreregistry_p.h \
            httpclient/creporterhttpclient_p.h \
            httpclient/creporteruploadengine_p.h \
            settings/creportersettingsbase_p.h \
            settings/creportersettingsinit_p.h \

TARGET = $$qtLibraryTarget(crashreporter)

target.path += $$[QT_INSTALL_LIBS]

headers.files = \
	$$PUBLIC_HEADERS \
	autouploader_interface.h
headers.CONFIG += no_check_exist
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
pkgconfig.path = $$[QT_INSTALL_LIBS]/pkgconfig
pkgconfig.depends = createpkgconfig

INSTALLS += target \
            pkgconfig \
            headers \
