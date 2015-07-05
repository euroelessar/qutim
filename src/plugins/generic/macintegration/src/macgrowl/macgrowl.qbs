import "../../../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    pluginTags: 'osx'
    sourcePath: ""

    cpp.frameworkPaths: base.concat("/Library/Frameworks/")
    cpp.frameworks: [ "Growl", "Cocoa" ]
}
