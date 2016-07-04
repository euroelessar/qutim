import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    type: [ "dynamiclibrary", "installed_content" ]
    Depends { name: "qutim-adiumchat" }
    Depends { name: "qutim-adiumwebview" }

    Group {
        name: "QML files"
        files: "qml/**"
        qbs.install: true
        qbs.installDir: project.qutim_share_path + "/qml/"
        qbs.installSourceBase: "qml"
    }

    Group {
        name: "QML files"
        files: "src/qmlchat"
        qbs.install: true
        qbs.installDir: project.qutim_share_path + "/src/qmlchat/"
        qbs.installSourceBase: "src/qmlchat"
    }

    Group {
        fileTags: "dummy"
        name: "QML files [themes]"
        files: "src/qmlchat/**"
    }
}
