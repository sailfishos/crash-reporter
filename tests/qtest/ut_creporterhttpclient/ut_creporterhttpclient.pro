include(../ut_common_top.pri)

CLIENT_SRC_DIR = $${CREPORTER_SRC_DIR}/libs/httpclient

CONFIG += meegotouch
QT -= network

TARGET = ut_creporterhttpclient

INCLUDEPATH += .  \
               $${CREPORTER_STUBS_DIR} \
               $${CLIENT_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs \
               $${CREPORTER_SRC_DIR}/libs/utils \
               $${CREPORTER_SRC_DIR}/libs/settings \
              /usr/include/qt4/QtNetwork

DEPENDPATH += $$INCLUDEPATH 

TEST_STUBS += $${CREPORTER_STUBS_DIR}/qnetworkreply.cpp \
              $${CREPORTER_STUBS_DIR}/qnetworkaccessmanager.cpp \

TEST_SOURCES += $${CLIENT_SRC_DIR}/creporterhttpclient.cpp \


HEADERS +=  $${CLIENT_SRC_DIR}/creporterhttpclient.h \
            $${CLIENT_SRC_DIR}/creporterhttpclient_p.h \
            $${CREPORTER_SRC_DIR}/libs/utils/creporterutils.h \
            $${CREPORTER_SRC_DIR}/libs/settings/creporterapplicationsettings.h \
            $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit_p.h \
            $${CREPORTER_STUBS_DIR}/qnetworkaccessmanager.h \
            $${CREPORTER_STUBS_DIR}/qnetworkreply.h \
            ut_creporterhttpclient.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterapplicationsettings.cpp \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit.cpp \
           $${CREPORTER_SRC_DIR}/libs/utils/creporterutils.cpp \
           ut_creporterhttpclient.cpp \

include(../ut_coverage.pri)
