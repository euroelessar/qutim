import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: [ "dynamiclibrary", "installed_content" ]
    Depends { name: "qutim-adiumchat" }
    Depends { name: "qutim-adiumwebview" }

    Group {
        name: "QML files"
        files: "qml"
        qbs.install: true
        qbs.installDir: project.qutim_share_path
    }

    Group {
        name: "QML files"
        files: "src/qmlchat"
        qbs.install: true
        qbs.installDir: project.qutim_share_path
    }
}
