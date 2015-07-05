# -------------------------------------------------
# Project created by QtCreator 2009-07-06T02:05:53
# -------------------------------------------------
QT += xml
QT -= gui
TARGET = mirgen
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
	../../libs/QZipReader/qzip.cpp
HEADERS += ../libs/QZipReader/qzipreader.h \
	../../libs/QZipReader/qzipwriter.h
INCLUDEPATH += ./ \
	../../src \
	../../src/utils \
	/usr/include \
	../../libs
