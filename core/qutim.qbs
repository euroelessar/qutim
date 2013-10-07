import qbs.base

Application {
	name: "qutim"

    property bool installSoundTheme: true

    destinationDirectory: "bin"
    cpp.rpaths: ["$ORIGIN/../lib"] //FIXME find normal way to deployment
    cpp.defines: [
        "QUTIM_PLUGIN_NAME=\"qutim\""
    ]

	Depends { name: "cpp" }
    Depends { name: "libqutim" }
//    Depends { name: "deploy" }
    Depends { name: "Qt"; submodules: [ 'core', 'gui', 'network', "script" ] }
    Depends { name: "Qt.widgets"; condition: Qt.core.versionMajor === 5 }
    Depends { name: "Qt.declarative"; condition: project.declarativeUi }
	
    files: [
        "main.cpp"
    ]

    Group {
        name: "Source"
        prefix: project.declarativeUi ? "src/declarative/" : "src/widgets/"
        files: [
            "*.h",
            "*.cpp",
            "*.ui"
        ]
    }

    Group {
        name: "Mac-specific"
        condition: qbs.targetOS.contains("osx")
        fileTags: "plugins"
//        prefix: deploy.pluginPath
//        files: [
//            "imageformats", "iconengines",
//            //"crypto",
//            "phonon_backend",
//            "bearer", "codecs",
//            "graphicssystems", "sqldrivers"
//        ]
    }
}
