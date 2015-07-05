import "../../../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    pluginTags: 'osx'
    sourcePath: ""

    cpp.frameworks: [ "Cocoa" ]
    Depends {
        condition: qbs.targetOS.contains('osx')
        name: "Qt.macextras"
    }
}
