import qbs.base 1.0

Product {
    name: "qutimplugin"
    type: ["dynamiclibrary", "installed_content"]

    property string uri: "org.qutim"

    cpp.rpaths: ["$ORIGIN/../../../../lib"]
    cpp.createSymlinks: false

    files: [
        "src/**/*.cpp",
        "src/**/*.h"
    ]

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: [ 'core', 'gui', 'network', 'script', 'quick', 'widgets' ] }
    Depends { name: "libqutim" }

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
