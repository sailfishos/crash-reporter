include(../../crash-reporter-conf.pri)

TEMPLATE = subdirs

QT -= gui

SUBDIRS =

# testsxml.target = $$OUT_PWD/tests.xml
# testsxml.commands = $$PWD/generate_tests_xml.sh > $$testsxml.target
# QMAKE_EXTRA_TARGETS += testsxml

# target.path = $$CREPORTER_UI_TESTS_INSTALL_DOCS
# target.files = $$testsxml.target
# target.depends = testsxml
# target.CONFIG += no_check_exist

testsxml.path = $$CREPORTER_UI_TESTS_INSTALL_DOCS
testsxml.files = tests.xml

test-dcpapplet.path = $$CREPORTER_UI_TESTS_INSTALL_DOCS
test-dcpapplet.files = test-dcpapplet 

test-crash-reporter-ui.path = $$CREPORTER_UI_TESTS_INSTALL_DOCS
test-crash-reporter-ui.files = test-crash-reporter-ui

INSTALLS += test-dcpapplet \
            test-crash-reporter-ui \
            testsxml \
