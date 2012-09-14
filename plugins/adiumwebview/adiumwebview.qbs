import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: ['installed_content', 'dynamiclibrary']

    Depends { name: "adiumchat" }
    Depends { name: "qt.webkit" }
    Depends { name: "qutim-adiumwebview" }
    Depends { name: "artwork" }
    Depends { name: "qutimscope" }

    Group {
        fileTags: [ "artwork" ]
        qutimscope.basePath: ''
        prefix: "data/webview/"
        files: '*'
        recursive: true
    }
}
