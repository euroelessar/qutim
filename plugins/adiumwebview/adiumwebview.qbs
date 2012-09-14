import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: ['installed_content', 'dynamiclibrary']

    Depends { name: "adiumchat" }
    Depends { name: "qt.webkit" }
    Depends { name: "qutim-adiumwebview" }
    Depends { name: "artwork" }

    Group {
        fileTags: [ "artwork" ]
        artwork.basePath: '.'
        prefix: "data/webview/"
        files: '*'
        recursive: true
    }
}
