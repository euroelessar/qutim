# -------------------------------------------------
# Project created by QtCreator 2009-07-05T20:29:59
# -------------------------------------------------
QT += xml
TARGET = packgen
TEMPLATE = app
SOURCES += main.cpp \
    generatorwindow.cpp \
    ../../src/plugxmlhandler.cpp \
    ../../src/plugpackage.cpp \
    ../../src/utils/plugversion.cpp \
    pluglibinfo.cpp \
    ../../libs/QZipReader/qzip.cpp \
    choosecategorypage.cpp \
    choosepathpage.cpp \
    configpackagepage.cpp
HEADERS += generatorwindow.h \
    ../../src/plugxmlhandler.h \
    ../../src/plugpackage.h \
    ../../src/utils/plugversion.h \
    pluglibinfo.h \
    ../../libs/QZipReader/qzipreader.h \
    ../../libs/QZipReader/qzipwriter.h \
    choosecategorypage.h \
    choosepathpage.h \
    configpackagepage.h
FORMS += choosecategorypage.ui \
    choosepathpage.ui \
    configpackagepage.ui
INCLUDEPATH += ./ \
    ../../src \
    ../../src/utils \
    /usr/include \
    ../../libs
