import "../../../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    pluginTags: 'osx'
    sourcePath: ""

    cpp.frameworks: [ "Cocoa" ]
}
