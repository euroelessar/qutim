import "../UreenPlugin.qbs" as UreenPlugin

Project {
    UreenPlugin {
        type: [ "dynamiclibrary", "installed_content" ]
        Depends { name: "qutim-adiumchat" }
        Depends { name: "Qt.webkit" }
        Depends { name: "Qt.webkitwidgets" }
        Depends { name: "qutim-adiumwebview" }
        Depends { name: "artwork" }

        Group {
            fileTags: [ "artwork" ]
            artwork.basePath: "./"
            prefix: "data/"
            files: "**"
        }
    }

    references: [
        "lib/qutim-adiumwebview.qbs"
    ]
}
