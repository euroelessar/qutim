import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    type: [ "dynamiclibrary", "installed_content" ]

    Depends { name: "qutim-adiumwebview" }
    Depends { name: "Qt.websockets" }
    Depends { name: "Qt.webchannel" }

    Group {
        name: "QML files"
        files: [
            "qml"
        ]
        qbs.install: true
        qbs.installDir: project.qutim_share_path
    }
}
