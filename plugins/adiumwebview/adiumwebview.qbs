import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: [ "dynamiclibrary", "installed_content" ]
    Depends { name: "qutim-adiumchat" }
    Depends { name: "Qt.webkit" }
    Depends { name: "qutim-adiumwebview" }
    Depends { name: "artwork" }

    Group {
        fileTags: [ "artwork" ]
        artwork.basePath: "./"
        prefix: "data/"
        files: "**"
    }
}
