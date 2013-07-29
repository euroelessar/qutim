import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
    name: "Qtsolutions"

    Depends { name: "Qt"; submodules: [ 'core' ] }

    files: [
        "qtsolutions/qtlocalpeer.h",
        "qtsolutions/qtlockedfile.h",
        "qtsolutions/qtlocalpeer.cpp",
        "qtsolutions/qtlockedfile.cpp",
    ]

    Group {
        name: "Windows-specific"
        condition: qbs.targetOS.contains("windows")
        files: "qtsolutions/qtlockedfile_win.cpp"
    }
    Group {
        name: "Unix-specific"
        condition: qbs.targetOS.contains("unix")
        files: "qtsolutions/qtlockedfile_unix.cpp"
    }
}
