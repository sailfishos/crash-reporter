include(../../crash-reporter-conf.pri)

TEMPLATE = app
QT += testlib dbus
CONFIG += debug

CREPORTER_SRC_DIR = ../../../src
CREPORTER_STUBS_DIR = ../stubs
CREPORTER_TESTLIB_DIR = ../testlib

include(testlib/testlib.pri)

INCLUDEPATH += . \
               $$CREPORTER_STUBS_DIR \
               $${CREPORTER_TESTLIB_DIR} \

DEPENDPATH += $$INCLUDEPATH \

target.path = $$CREPORTER_TESTS_INSTALL_LIBS

INSTALLS += target \

QMAKE_CLEAN += $(OBJECTS_DIR)/*.gcno
