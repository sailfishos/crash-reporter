# This file is a part of crash-reporter.
#
# Copyright (C) 2014 Jolla Ltd.
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
TARGET = crash-reporter-journalspy

QT = core
CONFIG += link_pkgconfig

INCLUDEPATH += \
	../libs \
	../libs/logger \
	../libs/settings \
	../libs/utils \

HEADERS = \
	journalspy.h

SOURCES = \
	main.cpp \
	journalspy.cpp \

LIBS += \
	../../lib/libcrashreporter.so \

PKGCONFIG += \
	libsystemd \

target.path = $$CREPORTER_SYSTEM_LIBEXEC

INSTALLS = target
