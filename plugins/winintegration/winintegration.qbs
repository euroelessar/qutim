import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    name: "winintegration"
    pluginTags: 'windows'
    Depends { condition: qbs.targetOS.contains("windows"); name: "Qt.winextras" }
    cpp.dynamicLibraries: ["user32", "shell32", "dwmapi", "ole32", "gdi32", "uuid" ]
    cpp.includePaths: [ "./libs/include" ]

    Group {
        name: "Specific source"
        prefix: (sourcePath !== '' ? sourcePath + '/' : '') + '**/'
        files: [ "subplugins/win7taskbar/*", "*.qrc", "libs/**" ]
    }

    condition: qbs.targetOS.contains("windows")
}

