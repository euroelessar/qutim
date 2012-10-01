import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
    name: "qtsolutions"

    Depends { name: "qt"; submodules: [ 'core' ] }

    files: [
        "qtsolutions/qtlocalpeer.h",
        "qtsolutions/qtlockedfile.h",
        "qtsolutions/qtlocalpeer.cpp",
        "qtsolutions/qtlockedfile.cpp",
    ]

    Group {
        condition: qbs.targetOs === "windows"
        files: "qtsolutions/qtlockedfile_win.cpp"
    }
    Group {
        condition: qbs.targetOs === "unix"
        files: "qtsolutions/qtlockedfile_unix.cpp"
    }
}
