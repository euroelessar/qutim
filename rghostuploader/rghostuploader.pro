#-------------------------------------------------
#
# Project created by QtCreator 2010-06-12T22:12:30
#
#-------------------------------------------------

QT       += network

TARGET = build/lib/rghostuploader
TEMPLATE = lib

OBJECTS_DIR = build/obj
UI_DIR = build/uic
MOC_DIR = build/moc
RCC_DIR = build/rcc

DEFINES += RGHOSTUPLOADER_LIBRARY

SOURCES += src/rghostuploaderplugin.cpp \
    src/datauploader.cpp \
    src/progresswindow.cpp \
    src/rghostuploaderbuffer.cpp

HEADERS += src/rghostuploaderplugin.h \
    src/datauploader.h \
    src/progresswindow.h \
    src/rghostuploaderbuffer.h

INCLUDEPATH += /usr/local/include \
               src/

FORMS += \
    src/progresswindow.ui

RESOURCES += \
    res.qrc

OTHER_FILES += \
    CMakeLists.txt \
    GPL_v3
