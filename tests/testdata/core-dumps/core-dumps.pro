include(../../../crash-reporter-conf.pri)
TEMPLATE = subdirs

SUBDIRS =

crash-reporter-tests.path = $$CREPORTER_TESTS_TESTDATA_INSTALL_LIBS
crash-reporter-tests.files += *.rcore.lzo

crash-reporter-ui-tests.path = $$CREPORTER_UI_TESTS_TESTDATA_INSTALL_LIBS
crash-reporter-ui-tests.files += *.rcore.lzo

INSTALLS += crash-reporter-tests \
            crash-reporter-ui-tests