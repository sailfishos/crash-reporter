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

DEPENDPATH += $$INCLUDEPATH \

TEST_STUBS +=

TEST_SOURCES += $${AUTOUPLOADER_SRC_DIR}/creporterautouploader.cpp \
                $${AUTOUPLOADER_SRC_DIR}/creporterautouploaderdbusadaptor.cpp \

HEADERS += $${AUTOUPLOADER_SRC_DIR}/creporterautouploader.h \
           $${AUTOUPLOADER_SRC_DIR}/creporterautouploaderdbusadaptor.h \
           ut_creporterautouploader.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           ut_creporterautouploader.cpp \

include(../ut_coverage.pri)
