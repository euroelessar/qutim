import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ''
    Properties {
        Dependes: "Qt.x11extras"
        condition: qbs.targetOS.contains("linux")
        cpp.dynamicLibraries: [ "xcb-screensaver", "xcb", "Qt5X11Extras" ]
    }
}
