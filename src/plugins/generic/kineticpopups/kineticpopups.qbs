import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    type: [ "dynamiclibrary", "installed_content" ]

    Group {
        files: "src/quickpopup/**"
        qbs.install: true
        qbs.installDir: project.qutim_share_path + "/quickpopup/"
        qbs.installSourceBase: "src/quickpopup/"
    }
}
