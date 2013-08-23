include(../ut_common_top.pri)

DAEMON_SRC_DIR = $${CREPORTER_SRC_DIR}/daemon

TARGET = ut_creportercoreregistry

LIBS += ../../../lib/libcrashreporter.so

INCLUDEPATH += . \
			   $${DAEMON_SRC_DIR} \
               $$CREPORTER_SRC_DIR/libs/coredir \
               $$CREPORTER_SRC_DIR/libs/utils \
               $$CREPORTER_SRC_DIR/libs \

DEPENDPATH += $$INCLUDEPATH \

TEST_STUBS += $${CREPORTER_STUBS_DIR}/mgconfitem_stub.cpp \

# unit
TEST_SOURCES += $${CREPORTER_SRC_DIR}/libs/coredir/creportercoredir.cpp \
	
HEADERS += $${CREPORTER_STUBS_DIR}/mgconfitem_stub.h \
           $${CREPORTER_SRC_DIR}/libs/coredir/creportercoredir.h \
           $${CREPORTER_SRC_DIR}/libs/coredir/creportercoredir_p.h \
		   $${CREPORTER_SRC_DIR}/libs/coredir/creportercoreregistry.h \
           $${CREPORTER_SRC_DIR}/libs/coredir/creportercoreregistry_p.h \
		   ut_creportercoreregistry.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $$TEST_STUBS \
           $${CREPORTER_SRC_DIR}/libs/coredir/creportercoreregistry.cpp \
		   ut_creportercoreregistry.cpp \

include(../ut_coverage.pri)
