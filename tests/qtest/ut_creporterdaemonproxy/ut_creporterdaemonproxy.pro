include(../ut_common_top.pri)

DAEMON_SRC_DIR = $${CREPORTER_SRC_DIR}/daemon

TARGET = ut_creporterdaemonproxy

LIBS += ../../../lib/libcrashreporter.so

INCLUDEPATH += . \
               $${CREPORTER_SRC_DIR}/libs/serviceif \
               $${CREPORTER_SRC_DIR}/libs/settings \
			   $${DAEMON_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs/utils \
               $${CREPORTER_SRC_DIR}/libs \
               $${CREPORTER_SRC_DIR}/libs/notification \
	
DEPENDPATH += $$INCLUDEPATH \

TEST_STUBS += $${CREPORTER_STUBS_DIR}/mgconfitem_stub.cpp \

TEST_SOURCES += $${DAEMON_SRC_DIR}/creporterdaemonadaptor.cpp \

	
HEADERS += $${CREPORTER_STUBS_DIR}/mgconfitem_stub.h \
		   $${DAEMON_SRC_DIR}/creporterdaemon.h \
           $${DAEMON_SRC_DIR}/creporterdaemon_p.h \
           $${DAEMON_SRC_DIR}/creporterdaemonadaptor.h \
           $${DAEMON_SRC_DIR}/creporterdaemonmonitor.h \
		   $${DAEMON_SRC_DIR}/creporterdaemoncoredir.h \
		   $${DAEMON_SRC_DIR}/creporterdaemoncoreregistry.h \
           $${DAEMON_SRC_DIR}/creporterdaemonmonitor_p.h \
           $${DAEMON_SRC_DIR}/creporterdaemoncoredir_p.h \
           $${DAEMON_SRC_DIR}/creporterdaemoncoreregistry_p.h \
           $${CREPORTER_SRC_DIR}/libs/notification/creporternotification.h \
           ut_creporterdaemonproxy.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           $${DAEMON_SRC_DIR}/creporterdaemon.cpp \
           $${DAEMON_SRC_DIR}/creporterdaemonmonitor.cpp \
           $${DAEMON_SRC_DIR}/creporterdaemoncoredir.cpp \
           $${DAEMON_SRC_DIR}/creporterdaemoncoreregistry.cpp \
           ut_creporterdaemonproxy.cpp \

include(../ut_coverage.pri)
