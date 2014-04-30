include(../ut_common_top.pri)

SETTINGS_SRC_DIR = $${CREPORTER_SRC_DIR}/libs/settings

QT -= gui

TARGET = ut_creporterprivacysettingsmodel

INCLUDEPATH += \
	$$SETTINGS_SRC_DIR \
	$$CREPORTER_SRC_DIR/libs \
	$$CREPORTER_SRC_DIR/libs/serviceif \
	$$CREPORTER_SRC_DIR/libs/utils \

TEST_SOURCES += $${SETTINGS_SRC_DIR}/creporterprivacysettingsmodel.cpp \
                $${SETTINGS_SRC_DIR}/creportersettingsbase.cpp \
                $${SETTINGS_SRC_DIR}/creportersettingsinit.cpp \

HEADERS += $${SETTINGS_SRC_DIR}/creporterprivacysettingsmodel.h \
            $${SETTINGS_SRC_DIR}/creportersettingsinit_p.h \
            $${SETTINGS_SRC_DIR}/creportersettingsbase_p.h \
            $${SETTINGS_SRC_DIR}/creportersettingsbase.h \
           $$CREPORTER_SRC_DIR/libs/autouploader_interface.h \
           $$CREPORTER_SRC_DIR/libs/coredir/creportercoredir.h \
           $$CREPORTER_SRC_DIR/libs/coredir/creportercoreregistry.h \
           $$CREPORTER_SRC_DIR/libs/utils/creporterutils.h \
            ut_creporterprivacysettingsmodel.h \

SOURCES += \
	$$TEST_SOURCES \
	ut_creporterprivacysettingsmodel.cpp \
	$$CREPORTER_SRC_DIR/libs/autouploader_interface.cpp \
	$$CREPORTER_SRC_DIR/libs/coredir/creportercoredir.cpp \
	$$CREPORTER_SRC_DIR/libs/coredir/creportercoreregistry.cpp \
	$$CREPORTER_SRC_DIR/libs/utils/creporterutils.cpp \

include(../ut_coverage.pri)
