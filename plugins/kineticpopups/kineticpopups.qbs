import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: [ "dynamiclibrary", "installed_content" ]

    Depends { name: 'qt.quick1' }
    Depends { name: "artwork" }

    Group {
        fileTags: [ "artwork" ]
        artwork.basePath: "src"
        prefix: "src/quickpopup/"
        recursive: true
        files: "*"
    }
}
