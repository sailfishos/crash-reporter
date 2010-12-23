include(../ut_common_top.pri)

TARGET = ut_creportersendalldialogplugin
CONFIG += meegotouch

PLUGIN_SRC_DIR = $${CREPORTER_SRC_DIR}/dialogplugins/sendallreportsdialogplugin

INCLUDEPATH += . \
               $${PLUGIN_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs \
               $${CREPORTER_SRC_DIR}/libs/dialoginterface \
               $${CREPORTER_SRC_DIR}/libs/utils \
               $${CREPORTER_SRC_DIR}/libs/settings \
               $${CREPORTER_SRC_DIR}/libs/notification \

DEPENDPATH += $$INCLUDEPATH \

TEST_STUBS += $${CREPORTER_STUBS_DIR}/creporterdialogserver_stub.cpp \

TEST_SOURCES += $${PLUGIN_SRC_DIR}/creportersendalldialogplugin.cpp \

HEADERS += $${PLUGIN_SRC_DIR}/creportersendalldialogplugin.h \
           $${PLUGIN_SRC_DIR}/creportersendalldialog.h \
           $${CREPORTER_SRC_DIR}/libs/dialoginterface/creporterdialogplugininterface.h \
           $${CREPORTER_SRC_DIR}/libs/dialoginterface/creporterdialogserverinterface.h \
           $${CREPORTER_STUBS_DIR}/creporterdialogserver_stub.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterapplicationsettings.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit_p.h \
           $${CREPORTER_SRC_DIR}/libs/notification/creporternotification.h \
           ut_creportersendalldialogplugin.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterapplicationsettings.cpp \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit.cpp \
           ut_creportersendalldialogplugin.cpp \

include(../ut_coverage.pri)
