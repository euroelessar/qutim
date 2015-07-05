QT += core \
    gui \
    sql
TARGET = sqlhistory
INCLUDEPATH += /usr/include \
    ../../include
TEMPLATE = lib
CONFIG += plugin
SOURCES += sqlhistory.cpp \
	sqlengine.cpp \
    historysettings.cpp \
    historywindow.cpp
HEADERS += sqlhistory.h \
	sqlengine.h \
    historysettings.h \
    historywindow.h
FORMS += historysettings.ui \
    historywindow.ui
RESOURCES += 
