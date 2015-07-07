import "../../../Integration.qbs" as Integration

Integration {
    pluginTags: 'osx'
    sourcePath: ""

    cpp.frameworkPaths: base.concat("/Library/Frameworks/")
    cpp.frameworks: [ "Growl", "Cocoa" ]
}
