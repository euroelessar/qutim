import qbs.base

Application {
	name: "qutim"

    property bool installSoundTheme: true

    destinationDirectory: qutim_bin_path
    cpp.rpaths: qbs.targetOS.contains("osx")
            ? ["@loader_path/..", "@executable_path/.."]
            : ["$ORIGIN", "$ORIGIN/..", "$ORIGIN/../lib"]
    cpp.defines: [
        "QUTIM_PLUGIN_NAME=\"qutim\""
    ]

	Depends { name: "cpp" }
    Depends { name: "libqutim" }
    Depends { name: "Qt"; submodules: [ 'core', 'gui', 'network', "script" ] }
    Depends { name: "Qt.widgets"; condition: Qt.core.versionMajor === 5 }
    Depends { name: "Qt.declarative"; condition: project.declarativeUi }

    consoleApplication: false
	
    files: [
        "main.cpp",
        "qutim.qrc",
        "qutim.rc"
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
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: qutim_bin_path
    }
}
