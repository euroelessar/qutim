import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: ['installed_content', 'dynamiclibrary']

    Depends { name: "vreen" }
    Depends { name: "vreenoauth" }
    Depends { name: "qt.quick1" }
    Depends { name: "qt.webkit" }
    Depends { name: "qutimscope" }
    Depends { name: "artwork" }

    cpp.includePaths: ["src"]

    Group {
        fileTags: [ "artwork" ]
        qutimscope.basePath: "share"
        prefix: "share/vphotoalbum/"
        files: '*'
        recursive: true
    }
}
