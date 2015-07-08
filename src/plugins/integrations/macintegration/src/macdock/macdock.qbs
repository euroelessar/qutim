import "../../../Integration.qbs" as Integration

Integration {
    pluginTags: 'osx'
    sourcePath: ""

    cpp.frameworks: [ "Cocoa" ]
    Depends {
        condition: qbs.targetOS.contains('osx')
        name: "Qt.macextras"
    }
}
