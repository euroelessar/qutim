import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    Depends { name: "Qt.xmlpatterns" }

    Group {
        name: "QML files"
        files: "qml"
        qbs.install: true
        qbs.installDir: project.qutim_share_path
    }
}
