include(../ut_common_top.pri)

TARGET = ut_creporteruploaddialogplugin
CONFIG += meegotouch

PLUGIN_SRC_DIR = $${CREPORTER_SRC_DIR}/dialogplugins/uploaddialogplugin

INCLUDEPATH += . \
               $${PLUGIN_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs \
               $${CREPORTER_SRC_DIR}/libs/dialoginterface \
               $${CREPORTER_SRC_DIR}/libs/httpclient \

DEPENDPATH += $$INCLUDEPATH \

TEST_STUBS += $${CREPORTER_STUBS_DIR}/creporterdialogserver_stub.cpp \

TEST_SOURCES += $${PLUGIN_SRC_DIR}/creporteruploaddialogplugin.cpp \

HEADERS += $${PLUGIN_SRC_DIR}/creporteruploaddialogplugin.h \
           $${PLUGIN_SRC_DIR}/creporteruploaddialog.h \
           $${CREPORTER_SRC_DIR}/libs/dialoginterface/creporterdialogplugininterface.h \
           $${CREPORTER_SRC_DIR}/libs/dialoginterface/creporterdialogserverinterface.h \
           $${CREPORTER_STUBS_DIR}/creporterdialogserver_stub.h \
           ut_creporteruploaddialogplugin.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           ut_creporteruploaddialogplugin.cpp \

include(../ut_coverage.pri)
