import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ''

    Group {
        name: "QML files"
        files: "qml"
        qbs.install: true
        qbs.installDir: project.qutim_share_path
    }
}
