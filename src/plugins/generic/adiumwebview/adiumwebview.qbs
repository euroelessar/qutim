import "../GenericPlugin.qbs" as GenericPlugin

Project {
    GenericPlugin {
        type: [ "dynamiclibrary", "installed_content" ]
        Depends { name: "qutim-adiumchat" }
        Depends { name: "Qt.webengine" }
        Depends { name: "Qt.webenginewidgets" }
        Depends { name: "qutim-adiumwebview" }

        Group {
            files: "data/**"
            qbs.install: true
            qbs.installDir: project.qutim_share_path + "data"
            qbs.installSourceBase: "data"
        }
    }

    references: [
        "lib/qutim-adiumwebview.qbs",
        "plugin/adiumplugin.qbs"
    ]
}
