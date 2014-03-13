import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: [ "dynamiclibrary", "installed_content" ]

    Group {
        files: "src/quickpopup"
        qbs.install: true
        qbs.installDir: project.qutim_share_path
    }
}
