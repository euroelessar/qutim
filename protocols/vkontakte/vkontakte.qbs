import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: ['installed_content', 'dynamiclibrary']

    Depends { name: "vreen" }
    Depends { name: "vreenoauth" }
    Depends { name: "Qt.webkit" }

    cpp.includePaths: ["src"]

    Group {
        files: "share/vphotoalbum"
        qbs.install: true
        qbs.installDir: project.qutim_share_path
    }
}
