import "../UreenPlugin.qbs" as UreenPlugin

Project {
    UreenPlugin {
        type: [ "dynamiclibrary", "installed_content" ]
        Depends { name: "qutim-adiumchat" }
        Depends { name: "Qt.webkit" }
        Depends { name: "Qt.webkitwidgets" }
        Depends { name: "qutim-adiumwebview" }

        Group {
            files: "data"
            qbs.install: true
            qbs.installDir: project.qutim_share_path
        }
    }

    references: [
        "lib/qutim-adiumwebview.qbs"
    ]
}
