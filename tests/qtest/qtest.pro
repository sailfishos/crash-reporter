TEMPLATE = subdirs

SUBDIRS =  ut_creporterdaemonmonitor \
          ut_creporterdaemon \
          ut_creporterdaemonproxy \
          ut_creportercoreregistry \
          ut_creportersettingsobserver \
          ut_creportercoredir \
          ut_creporterutils \
          ut_creporternwsessionmgr \
          ut_creporteruploaditem \
          ut_creporteruploadqueue \
          ut_creporteruploadengine \
          ut_creporterapplicationsettings \
          ut_creporterprivacysettingsmodel \

testsxml.target = $$OUT_PWD/tests.xml
testsxml.commands = $$PWD/generate_tests_xml.sh $$PWD > $$testsxml.target
QMAKE_EXTRA_TARGETS += testsxml

target.path = /usr/share/crash-reporter-tests
target.files = $$testsxml.target
target.depends = testsxml
target.CONFIG += no_check_exist

runtests.path = /usr/share/crash-reporter-tests
runtests.files = run_unit_tests.sh

INSTALLS += target \
            runtests
