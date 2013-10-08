import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ''
    Properties {
        condition: qbs.targetOS.contains("linux")
        cpp.dynamicLibraries: [ "xcb-screensaver", "xcb", "Qt5X11Extras" ]
    }
    Properties {
        condition: qbs.targetOS.contains("osx")
        cpp.frameworks: [ "Cocoa", "Carbon" ]
    }
}
