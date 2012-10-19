import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: ['installed_content', 'dynamiclibrary']

    Depends { name: "vreen" }
    Depends { name: "vreenoauth" }
    Depends { name: "qt.declarative" }
    Depends { name: "qt.webkit" }
    Depends { name: "artwork" }

    cpp.includePaths: ["src"]

    Group {
        fileTags: [ "artwork" ]
        artwork.basePath: "share"
        prefix: "share/vphotoalbum/"
        files: '*'
        recursive: true
    }
}
