import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: [ "dynamiclibrary", "installed_content" ]
    pluginTags: ['experimental']
    sourcePath: '.'

    Depends { name: "qt.quick1" }
    Depends { name: "qt.opengl" }
    Depends { name: "adiumchat" }
    Depends { name: "artwork" }

    Group {
        fileTags: [ "artwork" ]
        artwork.basePath: "./"
        prefix: "qmlchat/"
        recursive: true
        files: "*"
    }
}
