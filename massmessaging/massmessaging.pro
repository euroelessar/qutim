HEADERS += src/messaging.h \
    src/messagingdialog.h \
    src/manager.h \
    src/buddylistmanager.h \
    src/jsonfile.h
SOURCES += src/messaging.cpp \
    src/messagingdialog.cpp \
    src/manager.cpp \
    src/buddylistmanager.cpp \
    src/jsonfile.cpp
INCLUDEPATH += ../../include
unix:INCLUDEPATH += /usr/include
macx:INCLUDEPATH += /opt/local/include
CONFIG += qt \
    plugin
QT += core \
    gui
TEMPLATE = lib
FORMS += ui/messagingdialog.ui
TARGET = massmessaging
INSTALLS += target
unix:target.path += /usr/lib/qutim

include(3rdparty/k8json/k8json.pri)
