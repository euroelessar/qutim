import "../../../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    pluginTags: 'osx'
    sourcePath: ""

    cpp.frameworkPaths: base.concat("/Library/Frameworks/")
    cpp.frameworks: [ "Growl", "Cocoa" ]
}
