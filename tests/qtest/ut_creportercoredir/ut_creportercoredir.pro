include(../ut_common_top.pri)

DAEMON_SRC_DIR = $${CREPORTER_SRC_DIR}/daemon

QT -= gui

TARGET = ut_creportercoredir

LIBS += ../../../lib/libcrashreporter.so

INCLUDEPATH += . \
               $$CREPORTER_SRC_DIR/libs/coredir \
               $$CREPORTER_SRC_DIR/libs/utils \
               $$CREPORTER_SRC_DIR/libs \

DEPENDPATH += $$INCLUDEPATH \

TEST_SOURCES += $${CREPORTER_SRC_DIR}/libs/coredir/creportercoredir.cpp \

HEADERS += $${CREPORTER_SRC_DIR}/libs/coredir/creportercoredir.h \
           $${CREPORTER_SRC_DIR}/libs/coredir/creportercoredir_p.h \
           ut_creportercoredir.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           ut_creportercoredir.cpp \

include(../ut_coverage.pri)
