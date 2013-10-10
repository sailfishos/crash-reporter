#
# This file is part of crash-reporter
#
#  Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
#
#  Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
#  Author: Riku Halonen <riku.halonen@nokia.com>
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

QMAKE_TARGET_COMPANY = Nokia
QMAKE_TARGET_PRODUCT = Crash Reporter
QMAKE_TARGET_DESCRIPTION = Crash Reporter application.
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2010 Nokia

CREPORTER_SYSTEM_INCLUDE = /usr/include
CREPORTER_SYSTEM_SHARE = /usr/share
CREPORTER_SYSTEM_BIN = /usr/bin
CREPORTER_SYSTEM_DBUS_SERVICES = /usr/share/dbus-1/services
CREPORTER_SYSTEM_SYSTEMD_SERVICES = /usr/lib/systemd/user
CREPORTER_SYSTEM_ONESHOT = /usr/lib/oneshot.d
CREPORTER_SETTINGS_PATH = $${CREPORTER_SYSTEM_SHARE}/crash-reporter-settings
CREPORTER_DLG_PLUGINS_PATH = /usr/lib/crash-reporter/dialogplugins
CREPORTER_TESTS_TESTDATA_INSTALL_LIBS = $$[QT_INSTALL_LIBS]/crash-reporter-tests/testdata
CREPORTER_TESTS_TESTDATA_INSTALL_DOCS = $${CREPORTER_SYSTEM_SHARE}/crash-reporter-tests/testdata
CREPORTER_TESTS_INSTALL_LIBS = $$[QT_INSTALL_LIBS]/crash-reporter-tests
CREPORTER_UI_TESTS_TESTDATA_INSTALL_LIBS = $$[QT_INSTALL_LIBS]/crash-reporter-ui-tests/testdata
CREPORTER_UI_TESTS_TESTDATA_INSTALL_DOCS = $${CREPORTER_SYSTEM_SHARE}/crash-reporter-ui-tests/testdata
CREPORTER_UI_TESTS_INSTALL_DOCS = $${CREPORTER_SYSTEM_SHARE}/crash-reporter-ui-tests

#DEFINES += CREPORTER_SDK_HOST

# No debug output
contains(NO_DEBUG_OUTPUT, on) {
    DEFINES += QT_NO_DEBUG_OUTPUT
    DEFINES += QT_NO_WARNING_OUTPUT
}

include(version.pri)
CRVERSION = $$VERSION
DEFINES += CREPORTERVERSION=\\\"$$CRVERSION\\\"

OBJECTS_DIR = .obj
DESTDIR = .out
