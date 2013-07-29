import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: [ "dynamiclibrary", "installed_content" ]
    pluginTags: ['experimental']
    sourcePath: '.'

    Depends { name: "Qt.declarative" }
    Depends { name: "Qt.opengl" }
    Depends { name: "adiumchat" }
    Depends { name: "artwork" }

    Group {
        fileTags: [ "artwork" ]
        artwork.basePath: "./"
        prefix: "qmlchat/"
        files: "**"
    }
}
