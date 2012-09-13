import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: base.concat("installed_content")
    Depends { name: "adiumchat" }
    Depends { name: "qt.webkit" }
    Depends { name: "qutim-adiumwebview" }
    Depends { name: "artwork" }

    Group {
        fileTags: [ "artwork" ]
        artwork.basePath: "."
        prefix: "data/"
        recursive: true
        files: "*"
    }
}
