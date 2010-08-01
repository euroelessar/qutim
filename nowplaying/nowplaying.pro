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
               include

RESOURCES += \
    res.qrc

OTHER_FILES += \
    CMakeLists.txt \

HEADERS += \
    include/trackinfo.h \
    include/player.h \
    players/amarok/amarok.h \
    src/settingsui.h \
    src/nowplaying.h \
    src/settings_structures.h

SOURCES += \
    players/amarok/amarok.cpp \
    src/settingsui.cpp \
    src/nowplaying.cpp

FORMS += \
    src/settings.ui
