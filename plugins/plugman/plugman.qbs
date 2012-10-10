import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: [ "dynamiclibrary", "installed_content" ]

    Depends { name: "artwork" }
    Depends { name: "attica" }
    Depends { name: 'qt.quick1' }

    Group {
        fileTags: [ "artwork" ]
        artwork.basePath: "qml"
        prefix: "qml/plugman/"
        recursive: true
        files: "*"
    }
}
