include(../ut_common_top.pri)

TARGET = ut_creportermessageboxdialogplugin
CONFIG += meegotouch

PLUGIN_SRC_DIR = $${CREPORTER_SRC_DIR}/dialogplugins/messageboxdialogplugin

INCLUDEPATH += . \
               $${PLUGIN_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs \
               $${CREPORTER_SRC_DIR}/libs/dialoginterface \

DEPENDPATH += $$INCLUDEPATH \

TEST_STUBS += $${CREPORTER_STUBS_DIR}/creporterdialogserver_stub.cpp \

TEST_SOURCES += $${PLUGIN_SRC_DIR}/creportermessageboxdialogplugin.cpp \

HEADERS += $${PLUGIN_SRC_DIR}/creportermessageboxdialogplugin.h \
           $${CREPORTER_STUBS_DIR}/creporterdialogserver_stub.h \
           $${CREPORTER_SRC_DIR}/libs/dialoginterface/creporterdialogplugininterface.h \
           $${CREPORTER_SRC_DIR}/libs/dialoginterface/creporterdialogserverinterface.h \
           ut_creportermessageboxdialogplugin.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           ut_creportermessageboxdialogplugin.cpp \

include(../ut_coverage.pri)
