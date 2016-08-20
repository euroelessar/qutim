import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    Depends { name: "Qt.xmlpatterns" }

    Group {
        name: "QML files"
        files: "qml/**"
        qbs.install: true
        qbs.installDir: project.qutim_share_path + "/qml/"
        qbs.installSourceBase: "qml"
    }
}
