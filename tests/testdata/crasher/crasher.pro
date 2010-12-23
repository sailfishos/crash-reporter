include(../../../crash-reporter-conf.pri)

TEMPLATE = app

QT -= gui

TARGET = crasher

SOURCES = main.cpp

target.path = $$CREPORTER_TESTS_TESTDATA_INSTALL_LIBS

INSTALLS = target
