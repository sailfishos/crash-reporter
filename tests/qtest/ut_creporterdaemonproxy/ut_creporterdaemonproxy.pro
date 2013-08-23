include(../ut_common_top.pri)

DAEMON_SRC_DIR = $${CREPORTER_SRC_DIR}/daemon

TARGET = ut_creporterdaemonproxy

LIBS += ../../../lib/libcrashreporter.so

INCLUDEPATH += . \
               $${CREPORTER_SRC_DIR}/libs/coredir \
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
           $${DAEMON_SRC_DIR}/creporterdaemonmonitor_p.h \
           $${CREPORTER_SRC_DIR}/libs/coredir/creportercoredir_p.h \
           $${CREPORTER_SRC_DIR}/libs/coredir/creportercoreregistry_p.h \
           $${CREPORTER_SRC_DIR}/libs/coredir/creportercoredir.h \
           $${CREPORTER_SRC_DIR}/libs/coredir/creportercoreregistry.h \
           $${CREPORTER_SRC_DIR}/libs/notification/creporternotification.h \
           ut_creporterdaemonproxy.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           $${DAEMON_SRC_DIR}/creporterdaemon.cpp \
           $${DAEMON_SRC_DIR}/creporterdaemonmonitor.cpp \
           $${CREPORTER_SRC_DIR}/libs/coredir/creportercoredir.cpp \
           $${CREPORTER_SRC_DIR}/libs/coredir/creportercoreregistry.cpp \
           ut_creporterdaemonproxy.cpp \

include(../ut_coverage.pri)
