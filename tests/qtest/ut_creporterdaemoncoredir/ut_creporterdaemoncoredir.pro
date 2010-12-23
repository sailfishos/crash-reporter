include(../ut_common_top.pri)

DAEMON_SRC_DIR = $${CREPORTER_SRC_DIR}/daemon

QT -= gui

TARGET = ut_creporterdaemoncoredir

LIBS += ../../../lib/libcrashreporter.so

INCLUDEPATH += . \
               $${DAEMON_SRC_DIR} \
               $$CREPORTER_SRC_DIR/libs/utils \
               $$CREPORTER_SRC_DIR/libs \

DEPENDPATH += $$INCLUDEPATH \

TEST_SOURCES += $${DAEMON_SRC_DIR}/creporterdaemoncoredir.cpp \

HEADERS += $${DAEMON_SRC_DIR}/creporterdaemoncoredir.h \
           $${DAEMON_SRC_DIR}/creporterdaemoncoredir_p.h \
           ut_creporterdaemoncoredir.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           ut_creporterdaemoncoredir.cpp \

include(../ut_coverage.pri)
