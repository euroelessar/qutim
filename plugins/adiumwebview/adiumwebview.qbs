import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: [ "dynamiclibrary", "installed_content" ]
    Depends { name: "adiumchat" }
    Depends { name: "qt.webkit" }
    Depends { name: "qutim-adiumwebview" }
    Depends { name: "artwork" }
    Depends { name: "qutimscope" }

/*    Group {
        FileTags: [ "artwork" ] // Don't work
        qutimscope.basePath: "."
        prefix: "data/"
        recursive: true
        files: "*"
    }*/
}
