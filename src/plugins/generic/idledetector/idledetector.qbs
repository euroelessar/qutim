import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
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
