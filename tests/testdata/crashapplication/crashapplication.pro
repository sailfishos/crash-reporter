include(../../../crash-reporter-conf.pri)

QT -= gui
TEMPLATE = app

TARGET = crashapplication

SOURCES = main.cpp

target.path = $$CREPORTER_UI_TESTS_TESTDATA_INSTALL_LIBS

INSTALLS = target
