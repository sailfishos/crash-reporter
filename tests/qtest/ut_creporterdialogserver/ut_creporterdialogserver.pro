include(../ut_common_top.pri)

DLG_SERVER_SRC_DIR = $${CREPORTER_SRC_DIR}/dialogserver
CONFIG += meegotouch
QT += dbus

TARGET = ut_creporterdialogserver

INCLUDEPATH += . \
               $${DLG_SERVER_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs \
               $${CREPORTER_SRC_DIR}/libs/dialoginterface \
               $${CREPORTER_SRC_DIR}/libs/notification \
               $${CREPORTER_SRC_DIR}/libs/serviceif \
               $${CREPORTER_SRC_DIR}/libs/infobanner \

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
           ut_creporterdialogserver.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           $${CREPORTER_SRC_DIR}/libs/notification/creporternotification.cpp \
           $${CREPORTER_SRC_DIR}/libs/notification/creporternotificationadaptor.cpp \
           ut_creporterdialogserver.cpp \

include(../ut_coverage.pri)
