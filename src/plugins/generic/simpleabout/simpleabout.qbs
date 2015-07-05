import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    sourcePath: ''
    Group {
        name: "QML files"
        files: "qml"
        qbs.install: true
        qbs.installDir: project.qutim_share_path
    }
}
