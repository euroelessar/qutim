# Add more folders to ship with the application, here
folder_01.source = qml
folder_01.target = ../
DEPLOYMENTFOLDERS = folder_01

TARGET = qutim
DESTDIR = ../
QMAKE_CXXFLAGS += -std=c++11

QT += widgets qml quick

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH = $$OUT_PWD/../

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp

#copyCommand = @echo Doing some shit...
#copyCommand += && $(COPY_DIR) \"$$PWD/qml\" \"$$OUT_PWD/../\"

#ureendeployment.commands = $$copyCommand
#first.depends = $(first) ureendeployment
#export(first.depends)
#export(copydeploymentfolders.commands)
#QMAKE_EXTRA_TARGETS += first ureendeployment

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()
