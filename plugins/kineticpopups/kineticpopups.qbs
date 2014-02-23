import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: [ "dynamiclibrary", "installed_content" ]

    Depends { name: "Qt.quick" }

    Group {
        files: "src/quickpopup"
        qbs.install: true
        qbs.installDir: project.qutim_share_path
    }
}
