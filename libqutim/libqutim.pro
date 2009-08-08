# -------------------------------------------------
# Project created by QtCreator 2009-07-04T00:33:40
# -------------------------------------------------
OBJECTS_DIR = build/obj
UI_DIR = build/uic
MOC_DIR = build/moc
RCC_DIR = build/rcc
QT += network
TARGET = qutim
TEMPLATE = lib
DEFINES += LIBQUTIM_LIBRARY
SOURCES += tcpsocket.cpp \
    domaininfo.cpp \
    abstractlayer.cpp \
    settingslayer.cpp \
    modulemanager.cpp \
    plugin.cpp \
    deprecatedplugin.cpp \
    systeminfo.cpp \
    protocol.cpp \
    cryptoservice.cpp \
    abstractwizardpage.cpp \
    profilecreatorpage.cpp
HEADERS += tcpsocket.h \
    libqutim_global.h \
    libqutim_p.h \
    domaininfo.h \
    abstractlayer.h \
    settingslayer.h \
    modulemanager.h \
    plugin.h \
    deprecatedplugin_p.h \
    systeminfo.h \
    protocol.h \
    cryptoservice.h \
    abstractwizardpage.h \
    profilecreatorpage.h
include(../3rdparty/jdns/jdns.pri)
