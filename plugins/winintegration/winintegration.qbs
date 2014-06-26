import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    Depends { condition: qbs.targetOS.contains("windows"); name: "Qt.winextras" }
    pluginTags: 'windows' // Why this don't work?

    name: "winintegration"
    cpp.dynamicLibraries: ["user32", "shell32", "dwmapi", "ole32", "gdi32" ]
    cpp.includePaths: [ "./libs/include" ]
    files: [ "src/*", "subplugins/win7taskbar/*", "*.qrc", "libs/**" ]
}
