TEMPLATE = app
TARGET = test

QT -= xml gui
CONFIG += qt console warn_on
CONFIG += debug_and_release
#CONFIG += debug
#CONFIG += release

#QMAKE_CFLAGS_RELEASE ~= s/\-O./-Os
#QMAKE_CXXFLAGS_RELEASE ~= s/\-O./-Os

##QMAKE_CFLAGS_RELEASE ~= s/\-O./-O2
##QMAKE_CXXFLAGS_RELEASE ~= s/\-O./-O2

#QMAKE_CFLAGS_RELEASE += -march=native
#QMAKE_CXXFLAGS_RELEASE += -march=native
#QMAKE_CFLAGS_RELEASE += -mtune=native
#QMAKE_CXXFLAGS_RELEASE += -mtune=native

#QMAKE_LFLAGS_RELEASE += -s


DESTDIR = .
OBJECTS_DIR = _build/obj
UI_DIR = _build/uic
MOC_DIR = _build/moc
RCC_DIR = _build/rcc


DEFINES += K8JSON_INCLUDE_WRITER
DEFINES += K8JSON_INCLUDE_GENERATOR
include(../k8json.pri)

SOURCES += \
  $$PWD/test.cpp
