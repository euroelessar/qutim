import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: [ "dynamiclibrary", "installed_content" ]

    Depends { name: "Qt.declarative" }

    Group {
        files: "src/quickpopup"
        qbs.install: true
        qbs.installDir: project.qutim_share_path
    }
}
