TEMPLATE = lib
TARGET = icq
OBJECTS_DIR = build/obj
UI_DIR = build/uic
MOC_DIR = build/moc
RCC_DIR = build/rcc
DEPENDPATH += .
INCLUDEPATH += . \
    ../../../../libqutim/include \
    ../../../libqutim/include
unix:INCLUDEPATH += /usr/include
macx:INCLUDEPATH += /opt/local/include
CONFIG += qt \
    plugin
CONFIG -= embed_manifest_dll
QT += core \
    gui \
    network \
    xml
LIBS += -L../../../../libqutim \
    -L../../../libqutim \
    -lqutim

# Input
HEADERS += capabilty.h \
    dataunit.h \
    flap.h \
    icq_global.h \
    icqaccount.h \
    icqprotocol.h \
    md5login.h \
    oscarconnection.h \
    protocolnegotiation.h \
    roster.h \
    snac.h \
    snachandler.h \
    tlv.h \
    util.h \
    oscarplugin.h \
    messageplugin.h \
    icqcontact.h \
    icqcontact_p.h
SOURCES += capabilty.cpp \
    dataunit.cpp \
    flap.cpp \
    icqaccount.cpp \
    icqprotocol.cpp \
    md5login.cpp \
    oscarconnection.cpp \
    protocolnegotiation.cpp \
    roster.cpp \
    snac.cpp \
    snachandler.cpp \
    tlv.cpp \
    util.cpp \
    oscarplugin.cpp \
    messageplugin.cpp \
    icqcontact.cpp
