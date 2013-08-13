import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: [ "dynamiclibrary", "installed_content" ]

    Depends { name: "Qt.declarative" }
    Depends { name: "artwork" }

    Group {
        fileTags: [ "artwork" ]
        artwork.basePath: "src"
        prefix: "src/quickpopup/"
        files: "**"
    }
}
