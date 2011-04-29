include(../ut_common_top.pri)

TARGET = ut_creporternotifydialogplugin
CONFIG += meegotouch

LIBS += -lduicontrolpanel \

QMAKE_INCDIR += $${CREPORTER_SYSTEM_INCLUDE}/duicontrolpanel \

PLUGIN_SRC_DIR = $${CREPORTER_SRC_DIR}/dialogplugins/crashreportnotifydialogplugin

INCLUDEPATH += . \
               $${PLUGIN_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs \
               $${CREPORTER_SRC_DIR}/libs/dialoginterface \
               $${CREPORTER_SRC_DIR}/libs/utils \
               $${CREPORTER_SRC_DIR}/libs/settings \
               $${CREPORTER_SRC_DIR}/libs/notification \
               $${CREPORTER_SRC_DIR}/libs/infobanner \

DEPENDPATH += $$INCLUDEPATH \

TEST_STUBS += $${CREPORTER_STUBS_DIR}/duicontrolpanelif_stub.cpp \
              $${CREPORTER_STUBS_DIR}/creporterdialogserver_stub.cpp \

TEST_SOURCES += $${PLUGIN_SRC_DIR}/creporternotifydialogplugin.cpp \

HEADERS += $${PLUGIN_SRC_DIR}/creporternotifydialogplugin.h \
           $${PLUGIN_SRC_DIR}/creporternotificationdialog.h \
           $${CREPORTER_SRC_DIR}/libs/dialoginterface/creporterdialogplugininterface.h \
           $${CREPORTER_SRC_DIR}/libs/dialoginterface/creporterdialogserverinterface.h \
           $${CREPORTER_STUBS_DIR}/creporterdialogserver_stub.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterapplicationsettings.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterprivacysettingsmodel.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit_p.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase_p.h \
           $${CREPORTER_SRC_DIR}/libs/infobanner/creporterinfobanner.h \
           $${CREPORTER_SRC_DIR}/libs/notification/creporternotification.h \
           ut_creporternotifydialogplugin.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterapplicationsettings.cpp \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit.cpp \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterprivacysettingsmodel.cpp \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase.cpp \
           ut_creporternotifydialogplugin.cpp \

include(../ut_coverage.pri)
