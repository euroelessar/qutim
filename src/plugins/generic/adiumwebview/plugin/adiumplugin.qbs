import qbs.base 1.0

Product {
    name: "adiumviewplugin"
    type: ["dynamiclibrary", "installed_content"]

    property string uri: "org.qutim.adiumwebview"

    cpp.rpaths: ["$ORIGIN/../../../../../lib"]
    cpp.createSymlinks: false

    files: [
        "src/**/*.cpp",
        "src/**/*.h"
    ]

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: [ 'core', 'gui', 'network', 'script', 'quick', 'widgets' ] }
    Depends { name: "libqutim" }
    Depends { name: "qutim-adiumwebview" }

    Group {
        name: "QML files"

        qbs.installDir: project.qutim_share_path
        qbs.install: true
        files: "imports"
    }
    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.qutim_share_path + "/imports/" + uri.replace(/\./g, "/")
    }
}
