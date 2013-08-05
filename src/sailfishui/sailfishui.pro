TEMPLATE = subdirs

SUBDIRS += plugin

qmlpages.path = /usr/share/jolla-settings/pages/crash-reporter
qmlpages.files = *.qml

plugin_entry.path = /usr/share/jolla-settings/entries
plugin_entry.files = crash-reporter.json

INSTALLS += plugin_entry qmlpages

OTHER_FILES += *.qml *.json
