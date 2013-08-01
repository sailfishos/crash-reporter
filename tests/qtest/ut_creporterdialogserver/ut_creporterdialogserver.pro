include(../ut_common_top.pri)

DLG_SERVER_SRC_DIR = $${CREPORTER_SRC_DIR}/dialogserver
QT += dbus

TARGET = ut_creporterdialogserver

INCLUDEPATH += . \
               $${DLG_SERVER_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs \
               $${CREPORTER_SRC_DIR}/libs/dialoginterface \
               $${CREPORTER_SRC_DIR}/libs/notification \
               $${CREPORTER_SRC_DIR}/libs/serviceif \
               $${CREPORTER_SRC_DIR}/libs/infobanner \
               $${CREPORTER_SRC_DIR}/libs/settings \

DEPENDPATH += $$INCLUDEPATH \

# stubs
TEST_STUBS += \

# unit
TEST_SOURCES += $${DLG_SERVER_SRC_DIR}/creporterdialogserver.cpp \
                $${DLG_SERVER_SRC_DIR}/creporterdialogserverdbusadaptor.cpp \
                $${CREPORTER_SRC_DIR}/libs/serviceif/creporterdialogserverproxy.cpp \

HEADERS += $${DLG_SERVER_SRC_DIR}/creporterdialogserver.h \
           $${DLG_SERVER_SRC_DIR}/creporterdialogserver_p.h \
           $${DLG_SERVER_SRC_DIR}/creporterdialogserverdbusadaptor.h \
           $${CREPORTER_SRC_DIR}/libs/dialoginterface/creporterdialogplugininterface.h \
           $${CREPORTER_SRC_DIR}/libs/dialoginterface/creporterdialogserverinterface.h \
           $${CREPORTER_SRC_DIR}/libs/notification/creporternotification.h \
           $${CREPORTER_SRC_DIR}/libs/notification/creporternotification_p.h \
           $${CREPORTER_SRC_DIR}/libs/notification/creporternotificationadaptor.h \
           $${CREPORTER_SRC_DIR}/libs/serviceif/creporterdialogserverproxy.h \
           $${CREPORTER_SRC_DIR}/libs/infobanner/creporterinfobanner.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterapplicationsettings.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterprivacysettingsmodel.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit_p.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase_p.h \
           ut_creporterdialogserver.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           $${CREPORTER_SRC_DIR}/libs/notification/creporternotification.cpp \
           $${CREPORTER_SRC_DIR}/libs/notification/creporternotificationadaptor.cpp \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterapplicationsettings.cpp \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit.cpp \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterprivacysettingsmodel.cpp \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase.cpp \
           ut_creporterdialogserver.cpp \

include(../ut_coverage.pri)
