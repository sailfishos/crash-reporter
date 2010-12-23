include(../ut_common_top.pri)

TARGET = ut_creporteruploadqueue

HTTPCLIENT_SRC_DIR = $${CREPORTER_SRC_DIR}/libs/httpclient

INCLUDEPATH += . \
               $${HTTPCLIENT_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs \

DEPENDPATH += $$INCLUDEPATH \

TEST_SOURCES += $${HTTPCLIENT_SRC_DIR}/creporteruploadqueue.cpp \

HEADERS += $${HTTPCLIENT_SRC_DIR}/creporteruploadqueue.h \
           ut_creporteruploadqueue.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           ut_creporteruploadqueue.cpp \

include(../ut_coverage.pri)
