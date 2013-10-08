import "../../../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    pluginTags: 'osx-todo'
    sourcePath: ""

    cpp.frameworks: [ "Cocoa" ]

    Depends { name: "qutim-simplecontactlist" }
}
