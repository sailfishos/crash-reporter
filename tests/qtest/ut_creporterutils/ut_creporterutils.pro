include(../ut_common_top.pri)

TARGET = ut_creporterutils

INCLUDEPATH += . \
               $$CREPORTER_SRC_DIR/libs/serviceif \
               $$CREPORTER_SRC_DIR/libs/utils \
               $$CREPORTER_SRC_DIR/libs \

DEPENDPATH += $$INCLUDEPATH \

TEST_STUBS += \

# sources to be tested
TEST_SOURCES += $${CREPORTER_SRC_DIR}/libs/utils/creporterutils.cpp \

HEADERS += \
	$${CREPORTER_SRC_DIR}/libs/autouploader_interface.h \
	$${CREPORTER_SRC_DIR}/libs/utils/creporterutils.h \
	ut_creporterutils.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
	$${CREPORTER_SRC_DIR}/libs/autouploader_interface.cpp \
	ut_creporterutils.cpp \

include(../ut_coverage.pri)
