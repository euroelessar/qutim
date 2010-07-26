#-------------------------------------------------
#
# Project created by QtCreator 2010-06-12T22:12:30
#
#-------------------------------------------------

QT       += dbus

CONFIG += qtestlib

TARGET = build/lib/nowlplaying
TEMPLATE = lib

OBJECTS_DIR = build/obj
UI_DIR = build/uic
MOC_DIR = build/moc
RCC_DIR = build/rcc

DEFINES += NOWPLAYING_LIBRARY

INCLUDEPATH += /usr/local/include \
               src/

RESOURCES += \
    res.qrc

OTHER_FILES += \
    CMakeLists.txt \
    GPL_v3

HEADERS += \
    src/trackinfo.h \
    src/player.h \
    src/amarok.h \
    src/settingsui.h \
    src/nowplaying.h \
    src/settings_structures.h

SOURCES += \
    src/player.cpp \
    src/amarok.cpp \
    src/settingsui.cpp \
    src/nowplaying.cpp

FORMS += \
    src/settings.ui
