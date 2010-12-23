include(../ut_common_top.pri)

TARGET = ut_creporternotification
QT += dbus

NOTIFICATION_SRC_DIR = $${CREPORTER_SRC_DIR}/libs/notification

CONFIG += meegotouch

INCLUDEPATH += . \
               $${NOTIFICATION_SRC_DIR} \
               $${CREPORTER_SRC_DIR}/libs \

DEPENDPATH += $$INCLUDEPATH \

TEST_SOURCES += 	$${NOTIFICATION_SRC_DIR}/creporternotification.cpp \
                    $${NOTIFICATION_SRC_DIR}/creporternotificationadaptor.cpp \

HEADERS += $${NOTIFICATION_SRC_DIR}/creporternotification.h \
           $${NOTIFICATION_SRC_DIR}/creporternotification_p.h \
           $${NOTIFICATION_SRC_DIR}/creporternotificationadaptor.h \
           ut_creporternotification.h \

# unit test and sources
SOURCES += $$TEST_SOURCES \
           ut_creporternotification.cpp \

include(../ut_coverage.pri)
