TEMPLATE = lib
TARGET = core
QMAKE_CXXFLAGS += -std=c++11
QT += qml quick dbus
CONFIG += qt plugin
TARGET = $$qtLibraryTarget($$TARGET)
uri = org.qutim.core

DESTDIR = ../../imports/$$replace(uri, \., /)
copy_qmldir.target = $$DESTDIR/qmldir
copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
QMAKE_EXTRA_TARGETS += copy_qmldir
PRE_TARGETDEPS += $$copy_qmldir.target

# Input
SOURCES += \
    core_plugin.cpp \
    accountmanager.cpp \
    account.cpp \
    telepathy/telepathyaccountfactory.cpp \
    telepathy/telepathyaccount.cpp \
    quickaccountmanager.cpp

HEADERS += \
    core_plugin.h \
    accountmanager.h \
    functional.h \
    account.h \
    telepathy/telepathyaccountfactory.h \
    telepathy/telepathyaccount.h \
    account_p.h \
    quickaccountmanager.h

INCLUDEPATH += /usr/include/telepathy-qt5
LIBS += -ltelepathy-qt5

OTHER_FILES = qmldir

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

