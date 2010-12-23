LANGUAGES = # empty means only engineering English is needed.
CATALOGNAME = crash-reporter
SOURCEDIR = $$PWD/../src
TRANSLATIONDIR = $$PWD
LRELEASE_OPTIONS += -idbased # Defined here to prevent engineering english translations prefixed with '!!'.

include($$[QT_INSTALL_DATA]/mkspecs/features/meegotouch_defines.prf)
include($$[QT_INSTALL_DATA]/mkspecs/features/meegotouch_translations.prf)
