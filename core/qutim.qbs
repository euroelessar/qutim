import qbs.base 1.0

Product {
	name: "qutim"
    type: "application"
    destination: {
        if (qbs.targetOS === "mac")
            return product.name + ".app/Contents/MacOS";
        else
            return "bin";
    }
    property bool installSoundTheme: true

	Depends { name: "cpp" }
    Depends { name: "libqutim" }
    Depends { name: "qt"; submodules: [ 'core', 'gui', 'network', "script" ] }
    Depends { name: "qt.widgets"; condition: qt.core.versionMajor === 5 }
    Depends { name: "qt.quick1"; condition: project.declarativeUi }
	
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
    cpp.rpaths: ["$ORIGIN/../lib"] //FIXME find normal way to deployment
}
