import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    condition: Qt.webchannel.present && Qt.websockets.present

    type: [ "dynamiclibrary", "installed_content" ]

    Depends {
        name: "qutim-adiumwebview"
    }
    Depends {
        name: "Qt.websockets"
        required: false
    }
    Depends {
        name: "Qt.webchannel"
        required: false
    }

    Group {
        name: "QML files"
        files: [
            "qml/**"
        ]
        qbs.install: true
        qbs.installDir: project.qutim_share_path + "/qml/"
        qbs.installSourceBase: "qml"
    }
}
