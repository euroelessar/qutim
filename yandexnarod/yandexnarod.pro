TARGET = yandexnarod
HEADERS += yandexnarod.h \
    requestauthdialog.h \
    uploaddialog.h \
    yandexnarodsettings.h \
    yandexnarodmanage.h \
    yandexnarodnetman.h
SOURCES += yandexnarod.cpp \
    requestauthdialog.cpp \
    uploaddialog.cpp \
    yandexnarodsettings.cpp \
    yandexnarodmanage.cpp \
    yandexnarodnetman.cpp
INCLUDEPATH += ../../include /usr/include
CONFIG += qt \
    plugin
QT += core \
    gui \
		network \
		webkit
TEMPLATE = lib
RESOURCES += yandexnarod.qrc
FORMS += requestauthdialog.ui \
    uploaddialog.ui \
    yandexnarodsettings.ui \
    yandexnarodmanage.ui
TRANSLATIONS += yandexnarod_ru.ts
