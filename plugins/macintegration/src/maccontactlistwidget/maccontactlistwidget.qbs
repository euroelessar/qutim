import "../../../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    pluginTags: 'osx'
    sourcePath: ""

    cpp.frameworks: [ "Cocoa" ]

    Depends { name: "qutim-simplecontactlist" }
}
