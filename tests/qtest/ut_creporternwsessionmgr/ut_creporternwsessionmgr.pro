include(../ut_common_top.pri)

CLIENT_SRC_DIR = $${CREPORTER_SRC_DIR}/libs/httpclient

QT -= network

TARGET = ut_creporternwsessionmgr

INCLUDEPATH += .  \
               $${CREPORTER_STUBS_DIR} \
               $${CLIENT_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs

DEPENDPATH += $$INCLUDEPATH

TEST_STUBS += $${CREPORTER_STUBS_DIR}/qnetworkconfiguration.cpp \
            $${CREPORTER_STUBS_DIR}/qnetworksession.cpp

TEST_SOURCES += $${CLIENT_SRC_DIR}/creporternwsessionmgr.cpp

HEADERS +=  $${CLIENT_SRC_DIR}/creporternwsessionmgr.h \
            $${CREPORTER_STUBS_DIR}/qnetworkconfiguration.h \
            $${CREPORTER_STUBS_DIR}/qnetworkconfigmanager.h \
            $${CREPORTER_STUBS_DIR}/qnetworksession.h \
            ut_creporternwsessionmgr.h

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
	   ut_creporternwsessionmgr.cpp \

include(../ut_coverage.pri)
