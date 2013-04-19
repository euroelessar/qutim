TEMPLATE = lib
TARGET = statustheme
QT += qml quick
CONFIG += qt plugin
TARGET = $$qtLibraryTarget($$TARGET)
uri = org.qutim.status

QMAKE_CXXFLAGS += -std=c++11
DESTDIR = ../../imports/$$replace(uri, \., /)
copy_qmldir.target = $$DESTDIR/qmldir
copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
QMAKE_EXTRA_TARGETS += copy_qmldir
PRE_TARGETDEPS += $$copy_qmldir.target

# Input
SOURCES += \
    statustheme_plugin.cpp \
    statusthemeimageprovider.cpp

HEADERS += \
    statustheme_plugin.h \
    statusthemeimageprovider.h

OTHER_FILES = qmldir

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

