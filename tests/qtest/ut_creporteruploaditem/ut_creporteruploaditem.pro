include(../ut_common_top.pri)

TARGET = ut_creporteruploaditem

HTTPCLIENT_SRC_DIR = $${CREPORTER_SRC_DIR}/libs/httpclient

INCLUDEPATH += . \
               $${HTTPCLIENT_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs

DEPENDPATH += $$INCLUDEPATH \

TEST_SOURCES += $${HTTPCLIENT_SRC_DIR}/creporteruploaditem.cpp \

HEADERS += $${HTTPCLIENT_SRC_DIR}/creporteruploaditem.h \
           ut_creporteruploaditem.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           ut_creporteruploaditem.cpp \

include(../ut_coverage.pri)
