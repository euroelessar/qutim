import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    name: "winintegration"
    Depends { name: "Qt.winextras" }
    cpp.dynamicLibraries: ["user32", "shell32", "dwmapi", "ole32", "gdi32" ]
    cpp.includePaths: [ "./libs/include" ]
    files: [ "src/*", "subplugins/win7taskbar/*", "*.qrc", "libs/**" ]
}
