include(../ut_common_top.pri)
DAEMON_SRC_DIR = $${CREPORTER_SRC_DIR}/daemon
CONFIG += meegotouch
TARGET = ut_creporterdaemon
INCLUDEPATH += . \
    $${CREPORTER_SRC_DIR}/libs/serviceif \
    $${CREPORTER_SRC_DIR}/libs/settings \
    $${DAEMON_SRC_DIR} \
    $${CREPORTER_SRC_DIR}/libs/utils \
    $${CREPORTER_SRC_DIR}/libs \
    $${CREPORTER_STUBS_DIR} \
    $${CREPORTER_SRC_DIR}/dialogserver \
    $${CREPORTER_SRC_DIR}/libs/notification
DEPENDPATH += $$INCLUDEPATH
TEST_STUBS += $${CREPORTER_STUBS_DIR}/mgconfitem_stub.cpp
TEST_SOURCES += $${DAEMON_SRC_DIR}/creporterdaemon.cpp
HEADERS += $${CREPORTER_STUBS_DIR}/mgconfitem_stub.h \
    $${DAEMON_SRC_DIR}/creporterdaemon.h \
    $${DAEMON_SRC_DIR}/creporterdaemon_p.h \
    $${DAEMON_SRC_DIR}/creporterdaemonadaptor.h \
    $${DAEMON_SRC_DIR}/creporterdaemonmonitor.h \
    $${DAEMON_SRC_DIR}/creporterdaemonmonitor_p.h \
    $${DAEMON_SRC_DIR}/creporterdaemoncoredir.h \
    $${DAEMON_SRC_DIR}/creporterdaemoncoredir_p.h \
    $${DAEMON_SRC_DIR}/creporterdaemoncoreregistry.h \
    $${DAEMON_SRC_DIR}/creporterdaemoncoreregistry_p.h \
    $${CREPORTER_SRC_DIR}/dialogserver/creporterdialogserverdbusadaptor.h \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase.h \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase_p.h \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit_p.h \
    $${CREPORTER_SRC_DIR}/libs/settings/creporterprivacysettingsmodel.h \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsobserver.h \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsobserver_p.h \
    $${CREPORTER_SRC_DIR}/libs/utils/creporterutils.h \
    $${CREPORTER_SRC_DIR}/libs/serviceif/creporterdialogserverproxy.h \
    $${CREPORTER_SRC_DIR}/libs/serviceif/creporterautouploaderproxy.h \
    $${CREPORTER_SRC_DIR}/libs/notification/creporternotification.h \
    ut_creporterdaemon.h

# unit test and sources
SOURCES += $$TEST_SOURCES \
    $$TEST_STUBS \
    $${DAEMON_SRC_DIR}/creporterdaemonadaptor.cpp \
    $${DAEMON_SRC_DIR}/creporterdaemonmonitor.cpp \
    $${DAEMON_SRC_DIR}/creporterdaemoncoredir.cpp \
    $${DAEMON_SRC_DIR}/creporterdaemoncoreregistry.cpp \
    $${CREPORTER_SRC_DIR}/dialogserver/creporterdialogserverdbusadaptor.cpp \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsinit.cpp \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsbase.cpp \
    $${CREPORTER_SRC_DIR}/libs/settings/creporterprivacysettingsmodel.cpp \
    $${CREPORTER_SRC_DIR}/libs/settings/creportersettingsobserver.cpp \
    $${CREPORTER_SRC_DIR}/libs/utils/creporterutils.cpp \
    $${CREPORTER_SRC_DIR}/libs/serviceif/creporterdialogserverproxy.cpp \
    $${CREPORTER_SRC_DIR}/libs/serviceif/creporterautouploaderproxy.cpp \
    ut_creporterdaemon.cpp
include(../ut_coverage.pri)
