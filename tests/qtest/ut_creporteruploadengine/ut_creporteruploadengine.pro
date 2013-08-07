include(../ut_common_top.pri)

TARGET = ut_creporteruploadengine

HTTPCLIENT_SRC_DIR = $${CREPORTER_SRC_DIR}/libs/httpclient

INCLUDEPATH += . \
               $${HTTPCLIENT_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs \
               $${CREPORTER_SRC_DIR}/libs/settings \

DEPENDPATH += $$INCLUDEPATH \

DEFINES += CREPORTER_LIBBEARER_ENABLED

TEST_SOURCES += $${HTTPCLIENT_SRC_DIR}/creporteruploadengine.cpp \

HEADERS += $${HTTPCLIENT_SRC_DIR}/creporteruploadengine.h \
           $${HTTPCLIENT_SRC_DIR}/creporteruploadengine_p.h \
           $${HTTPCLIENT_SRC_DIR}/creporteruploadqueue.h \
           $${HTTPCLIENT_SRC_DIR}/creporteruploaditem.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterapplicationsettings.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase_p.h \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit_p.h \
           ut_creporteruploadengine.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           $${HTTPCLIENT_SRC_DIR}/creporteruploadqueue.cpp \
           $${HTTPCLIENT_SRC_DIR}/creporteruploaditem.cpp \
           $${CREPORTER_SRC_DIR}/libs/settings/creporterapplicationsettings.cpp \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase.cpp \
           $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit.cpp \
           ut_creporteruploadengine.cpp \

include(../ut_coverage.pri)
