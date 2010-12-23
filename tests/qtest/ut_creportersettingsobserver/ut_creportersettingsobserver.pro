include(../ut_common_top.pri)

SETTINGS_SRC_DIR = $${CREPORTER_SRC_DIR}/libs/settings

QT -= gui

TARGET = ut_creportersettingsobserver

INCLUDEPATH += $$SETTINGS_SRC_DIR \
               $${CREPORTER_SRC_DIR}/libs \

DEPENDPATH += $$INCLUDEPATH \

TEST_SOURCES += $${SETTINGS_SRC_DIR}/creportersettingsobserver.cpp \
	
HEADERS += $${SETTINGS_SRC_DIR}/creportersettingsobserver.h \
            $${SETTINGS_SRC_DIR}/creportersettingsobserver_p.h \
            ut_creportersettingsobserver.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           ut_creportersettingsobserver.cpp \

include(../ut_coverage.pri)
