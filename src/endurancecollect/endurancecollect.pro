# This file is part of crash-reporter
#
# Copyright (C) 2013 Jolla Ltd.
# Contact: Jakub Adam <jakub.adam@jollamobile.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# version 2.1 as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301 USA

include(../../crash-reporter-conf.pri)

TEMPLATE = app
TARGET = endurance-collect-daemon

CONFIG -= qt
CONFIG += link_pkgconfig

SOURCES = main.c

PKGCONFIG += \
    dbus-1 \
    libiphb

LIBS += -lrt

# qmake uses C++ linker by default, unnecessarily pulling in libstdc++
QMAKE_LINK = $$QMAKE_LINK_C

target.path = $$CREPORTER_SYSTEM_LIBEXEC

INSTALLS = target
