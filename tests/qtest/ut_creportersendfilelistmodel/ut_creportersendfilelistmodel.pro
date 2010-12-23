include(../ut_common_top.pri)

QT -= gui
CONFIG += meegotouch

PLUGIN_SRC_DIR = $${CREPORTER_SRC_DIR}/dialogplugins/sendselecteddialogplugin

TARGET = ut_creportersendfilelistmodel

INCLUDEPATH += $${PLUGIN_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs \
               $${CREPORTER_SRC_DIR}/libs/utils \

DEPENDPATH += $$INCLUDEPATH \

TEST_SOURCES += $${PLUGIN_SRC_DIR}/creportersendfilelistmodel.cpp \

HEADERS += $${PLUGIN_SRC_DIR}/creportersendfilelistmodel.h \
           $${CREPORTER_SRC_DIR}/libs/utils/creporterutils.h \
            ut_creportersendfilelistmodel.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           ut_creportersendfilelistmodel.cpp \

include(../ut_coverage.pri)
