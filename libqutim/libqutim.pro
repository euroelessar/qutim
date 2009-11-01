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
mac:LIBS += -framework CoreServices
INCLUDEPATH += ../ \
    ../sdk02/
SOURCES += tcpsocket.cpp \
    domaininfo.cpp \
    settingslayer.cpp \
    modulemanager.cpp \
    plugin.cpp \
    deprecatedplugin.cpp \
    systeminfo.cpp \
    protocol.cpp \
    cryptoservice.cpp \
    abstractwizardpage.cpp \
    profilecreatorpage.cpp \
    jsonfile.cpp \
    configbase.cpp \
    configbackend.cpp \
    event.cpp \
    localizedstring.cpp \
    metacontact.cpp \
    account.cpp \
    contact.cpp \
    objectgenerator.cpp \
    icon.cpp \
    event_test.cpp \
    notificationslayer.cpp \
    iconbackend.cpp \
    contactlist.cpp \
    message.cpp \
    abstractcontact.cpp \
    settingswidget.cpp \
    messagesession.cpp \
    iconloader.cpp \
    utils.cpp
HEADERS += tcpsocket.h \
    libqutim_global.h \
    domaininfo.h \
    settingslayer.h \
    modulemanager.h \
    plugin.h \
    deprecatedplugin_p.h \
    systeminfo.h \
    protocol.h \
    cryptoservice.h \
    abstractwizardpage.h \
    profilecreatorpage.h \
    jsonfile.h \
    configbase.h \
    configbase_p.h \
    configbackend.h \
    event.h \
    localizedstring.h \
    metacontact.h \
    account.h \
    contact.h \
    objectgenerator.h \
    icon.h \
    event_test_p.h \
    notificationslayer.h \
    iconbackend_p.h \
    contactlist.h \
    message.h \
    abstractcontact.h \
    settingswidget.h \
    settingslayer_p.h \
    messagesession.h \
    iconloader.h
include(../3rdparty/jdns/jdns.pri)
include(../3rdparty/k8json/k8json.pri)
include(../3rdparty/qticonloader/qticonloader.pri)
