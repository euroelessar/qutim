import qbs.base 1.0

Product {
	name: "qutim"

    property bool installSoundTheme: true

    type: ["application", "installed_content"]
    destination: {
        if (qbs.targetOS === "mac")
            return product.name + ".app/Contents/MacOS";
        else
            return "bin";
    }
    cpp.rpaths: ["$ORIGIN/../lib"] //FIXME find normal way to deployment

	Depends { name: "cpp" }
    Depends { name: "libqutim" }
    Depends { name: "deploy" }
    Depends { name: "qt"; submodules: [ 'core', 'gui', 'network', "script" ] }
    Depends { name: "qt.widgets"; condition: qt.core.versionMajor === 5 }
    Depends { name: "qt.declarative"; condition: project.declarativeUi }
	
    files: [
        "main.cpp"
    ]

    Group {
        prefix: project.declarativeUi ? "src/declarative/" : "src/widgets/"
        files: [
            "*.h",
            "*.cpp",
            "*.ui"
        ]
    }

    Group {
        condition: qbs.targetOS === "mac"
        fileTags: "plugins"
        prefix: deploy.pluginPath
        files: [
            "imageformats", "iconengines",
            //"crypto",
            "phonon_backend",
            "bearer", "codecs",
            "graphicssystems", "sqldrivers"
        ]
    }
}
