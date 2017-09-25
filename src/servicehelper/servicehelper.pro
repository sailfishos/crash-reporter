TEMPLATE = app
TARGET = crashreporter-servicehelper

CONFIG -= qt

SOURCES = main.c

# qmake uses C++ linker by default, unnecessarily pulling in libstdc++
QMAKE_LINK = $$QMAKE_LINK_C

target.path = /usr/libexec

INSTALLS = target
