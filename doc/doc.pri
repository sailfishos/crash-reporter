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
include(../crash-reporter-conf.pri)

DOXYGEN_BIN = $$system(which doxygen)
HTML_DOCS_DIR = doc/html
MAN_PAGES_DIR = doc/man/man1

system(mkdir -p $${OUT_PWD}/$${HTML_DOCS_DIR})
system(mkdir -p $${OUT_PWD}/$${MAN_PAGES_DIR})

# Documentation build target
doctarget.target = docs
doctarget.commands = INPUT=\"$${PWD}/../src/ $${PWD}/src/\" \
           DOC_SRC=\"$${PWD}/src/\" \
           IMAGE_SRC=\"$${PWD}/src/images/\" \  
           OUTPUT=\"$${OUT_PWD}/doc\" \
           $${DOXYGEN_BIN} $${PWD}/Doxyfile

QMAKE_EXTRA_TARGETS += doctarget

# Add documentation cleaning to clean target
doccleantarget.target = clean
doccleantarget.commands += rm -rf $${OUT_PWD}/$${HTML_DOCS_DIR}

QMAKE_EXTRA_TARGETS += doccleantarget

htmldocs.files = $${OUT_PWD}/$${HTML_DOCS_DIR}
htmldocs.path = $${CREPORTER_SYSTEM_SHARE}/doc/crash-reporter
# htmldocs.depends = docs

# Generate man pages.
mantarget.target = man
mantarget.commands = groff -man -Tascii $${PWD}/crash-reporter-ui.man > \
                     $${OUT_PWD}/$${MAN_PAGES_DIR}/crash-reporter-ui.1 && \
                     groff -man -Tascii $${PWD}/crash-reporter-daemon.man > \
                     $${OUT_PWD}/$${MAN_PAGES_DIR}/crash-reporter-daemon.1 && \
                     groff -man -Tascii $${PWD}/crash-reporter-autouploader.man > \
                     $${OUT_PWD}/$${MAN_PAGES_DIR}/crash-reporter-autouploader.1 \

QMAKE_EXTRA_TARGETS += mantarget

mancleantarget.target = clean
doccleantarget.commands = rm -rf $${OUT_PWD}/$${MAN_PAGES_DIR}

QMAKE_EXTRA_TARGETS += mancleantarget

manpages.path = $${CREPORTER_SYSTEM_SHARE}/man
manpages.files = $${OUT_PWD}/$${MAN_PAGES_DIR}
manpages.depends = man

INSTALLS += htmldocs \
            manpages


