include(../ut_common_top.pri)

DAEMON_SRC_DIR = $${CREPORTER_SRC_DIR}/daemon

TARGET = ut_creporterdaemoncoreregistry

LIBS += ../../../lib/libcrashreporter.so

INCLUDEPATH += . \
			   $${DAEMON_SRC_DIR} \
               $$CREPORTER_SRC_DIR/libs/utils \
               $$CREPORTER_SRC_DIR/libs \

DEPENDPATH += $$INCLUDEPATH \

TEST_STUBS += $${CREPORTER_STUBS_DIR}/mgconfitem_stub.cpp \

# unit
TEST_SOURCES +=	$${DAEMON_SRC_DIR}/creporterdaemoncoredir.cpp \
	
HEADERS += $${CREPORTER_STUBS_DIR}/mgconfitem_stub.h \
           $${DAEMON_SRC_DIR}/creporterdaemoncoredir.h \
           $${DAEMON_SRC_DIR}/creporterdaemoncoredir_p.h \
		   $${DAEMON_SRC_DIR}/creporterdaemoncoreregistry.h \
           $${DAEMON_SRC_DIR}/creporterdaemoncoreregistry_p.h \
		   ut_creporterdaemoncoreregistry.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           $${DAEMON_SRC_DIR}/creporterdaemoncoreregistry.cpp \
		   ut_creporterdaemoncoreregistry.cpp \

include(../ut_coverage.pri)
