import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: [ "dynamiclibrary", "installed_content" ]

    Depends { name: "artwork" }
    Depends { name: "attica" }
    Depends { name: 'qt.declarative' }
    Depends { name: 'qt.opengl' }

    Group {
        fileTags: [ "artwork" ]
        artwork.basePath: "qml"
        prefix: "qml/plugman/"
        recursive: true
        files: "*"
    }
}
