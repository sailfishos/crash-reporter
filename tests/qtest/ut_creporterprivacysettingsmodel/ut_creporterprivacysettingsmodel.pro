include(../ut_common_top.pri)

SETTINGS_SRC_DIR = $${CREPORTER_SRC_DIR}/libs/settings

QT -= gui

TARGET = ut_creporterprivacysettingsmodel

INCLUDEPATH += $$SETTINGS_SRC_DIR \
               $${CREPORTER_SRC_DIR}/libs \

DEPENDPATH += $$INCLUDEPATH \

TEST_SOURCES += $${SETTINGS_SRC_DIR}/creporterprivacysettingsmodel.cpp \
                $${SETTINGS_SRC_DIR}/creportersettingsbase.cpp \
                $${SETTINGS_SRC_DIR}/creportersettingsinit.cpp \

HEADERS += $${SETTINGS_SRC_DIR}/creporterprivacysettingsmodel.h \
            $${SETTINGS_SRC_DIR}/creportersettingsinit_p.h \
            $${SETTINGS_SRC_DIR}/creportersettingsbase_p.h \
            $${SETTINGS_SRC_DIR}/creportersettingsbase.h \
            ut_creporterprivacysettingsmodel.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           ut_creporterprivacysettingsmodel.cpp \

include(../ut_coverage.pri)
