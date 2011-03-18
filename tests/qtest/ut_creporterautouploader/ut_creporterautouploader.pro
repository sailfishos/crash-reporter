include(../ut_common_top.pri)

TARGET = ut_creporterautouploader
CONFIG += meegotouch
QT += dbus

AUTOUPLOADER_SRC_DIR = $${CREPORTER_SRC_DIR}/autouploader

INCLUDEPATH += . \
               $${AUTOUPLOADER_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs \
               $${CREPORTER_SRC_DIR}/libs/dialoginterface \
               $${CREPORTER_SRC_DIR}/libs/httpclient \
               $${CREPORTER_SRC_DIR}/libs/notification \
               $${CREPORTER_SRC_DIR}/libs/settings \

DEPENDPATH += $$INCLUDEPATH \

TEST_STUBS +=

TEST_SOURCES += $${AUTOUPLOADER_SRC_DIR}/creporterautouploader.cpp \
                $${AUTOUPLOADER_SRC_DIR}/creporterautouploaderdbusadaptor.cpp \

HEADERS += $${AUTOUPLOADER_SRC_DIR}/creporterautouploader.h \
           $${AUTOUPLOADER_SRC_DIR}/creporterautouploaderdbusadaptor.h \
           $${CREPORTER_SRC_DIR}/libs/notification/creporternotification.h \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase.h \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase_p.h \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit_p.h \
    $${CREPORTER_SRC_DIR}/libs/settings/creporterprivacysettingsmodel.h \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsobserver.h \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsobserver_p.h \
           ut_creporterautouploader.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           ut_creporterautouploader.cpp \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit.cpp \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase.cpp \
    $${CREPORTER_SRC_DIR}/libs/settings/creporterprivacysettingsmodel.cpp \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsobserver.cpp \

include(../ut_coverage.pri)
