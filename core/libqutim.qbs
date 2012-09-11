import qbs.base 1.0

DynamicLibrary {
    name: "libqutim"
    destination: "lib"
    
    property string versionMajor: project.qutim_version_major
    property string versionMinor: project.qutim_version_minor
    property string versionRelease: project.qutim_version_release
    property string versionPatch: project.qutim_version_patch
    property string version: project.qutim_version
    property string shareDir: {
        if (qbs.targetOS === "mac")
            return "/Resources/share";
        else
            return project.shareDir;
    }

    Depends { name: "qutim-headers" }
    Depends { name: "k8json" }
    Depends { name: "qxt" }
    Depends { name: "qxdg" }
    Depends { name: "qtdwm" }
    Depends { name: "qtsolutions" }
    Depends { name: "cpp" }
    Depends { name: "qt"; submodules: [ 'core', 'gui', 'network', 'script', 'quick1' ] }
    Depends { name: "qt.widgets"; condition: qt.core.versionMajor === 5 }
    Depends { name: "carbon"; condition: qbs.targetOS === 'mac' }
    Depends { name: "cocoa"; condition: qbs.targetOS === 'mac' }
    Depends { name: "windows.user32"; condition: qbs.targetOS === 'windows' }
    Depends { name: "windows.gdi32"; condition: qbs.targetOS === 'windows' } //in product module it's doesn't work
    Depends { name: "x11"; condition: qbs.targetOS === 'linux' }

    cpp.includePaths: [
        buildDirectory + "/include/qutim"
    ]

    cpp.dynamicLibraryPrefix: ""
    cpp.staticLibraryPrefix: ""
    cpp.defines: [
        "LIBQUTIM_LIBRARY",
        "QUTIM_SHARE_DIR=\"" + shareDir + "\"",
    ]

    ProductModule {
        property string basePath

        Depends { name: "cpp" }
        cpp.includePaths: [
            product.buildDirectory + "/include",
            "3rdparty/slidingstackedwidget",
            "3rdparty/flowlayout",
            "3rdparty/"
        ]
        cpp.defines: []
        Properties {
            condition: project.declarativeUi
            cpp.defines: outer.concat("QUTIM_DECLARATIVE_UI")
        }
        Properties {
            condition: project.singleProfile
            cpp.defines: outer.concat([ "QUTIM_SINGLE_PROFILE" ])
        }
    }

    files: [
        'libqutim/*.h',
        'libqutim/*.cpp',
        'libqutim/utils/*.h',
        'libqutim/utils/*.cpp'
    ]

    //TODO separate this libraries like qutim-adiumwebview
    Group {
        prefix: "3rdparty/slidingstackedwidget/"
        files: ["*.h", "*.cpp"]
    }
    Group {
        prefix: "3rdparty/flowlayout/"
        files: ["*.h", "*.cpp"]
    }
}
