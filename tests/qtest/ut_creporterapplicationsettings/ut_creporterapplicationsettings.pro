include(../ut_common_top.pri)

SETTINGS_SRC_DIR = $${CREPORTER_SRC_DIR}/libs/settings

QT -= gui

TARGET = ut_creporterapplicationsettings

INCLUDEPATH += $$SETTINGS_SRC_DIR \
               $${CREPORTER_SRC_DIR}/libs \

DEPENDPATH += $$INCLUDEPATH \

TEST_SOURCES += $${SETTINGS_SRC_DIR}/creporterapplicationsettings.cpp \
                $${SETTINGS_SRC_DIR}/creportersettingsinit.cpp \

HEADERS += $${SETTINGS_SRC_DIR}/creporterapplicationsettings.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase_p.h \
            $${SETTINGS_SRC_DIR}/creportersettingsinit_p.h \
            ut_creporterapplicationsettings.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase.cpp \
           ut_creporterapplicationsettings.cpp \

include(../ut_coverage.pri)
