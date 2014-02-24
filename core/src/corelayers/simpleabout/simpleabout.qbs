import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ''

    Depends { name: "Qt.quick" }

    Group {
        files: "qml"
        qbs.install: true
        qbs.installDir: project.qutim_share_path
    }
}
