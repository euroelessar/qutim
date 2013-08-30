import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ''
    cpp.dynamicLibraries: [ "xcb-screensaver", "xcb" ]
}
