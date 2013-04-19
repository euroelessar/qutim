TEMPLATE = lib
TARGET = icontheme
QT += qml quick
CONFIG += qt plugin
TARGET = $$qtLibraryTarget($$TARGET)
uri = org.qutim.icons

QMAKE_CXXFLAGS += -std=c++11
DESTDIR = ../../imports/$$replace(uri, \., /)
copy_qmldir.target = $$DESTDIR/qmldir
copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
QMAKE_EXTRA_TARGETS += copy_qmldir
PRE_TARGETDEPS += $$copy_qmldir.target

# Input
SOURCES += \
    desktoptheme_plugin.cpp \
    desktopthemeimageprovider.cpp

HEADERS += \
    desktoptheme_plugin.h \
    desktopthemeimageprovider.h

OTHER_FILES = qmldir

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

