include(../../../crash-reporter-conf.pri)
TEMPLATE = subdirs

SUBDIRS =

crash-reporter-ui-tests.path = $$CREPORTER_UI_TESTS_TESTDATA_INSTALL_DOCS
crash-reporter-ui-tests.files += *.conf

INSTALLS += crash-reporter-ui-tests
