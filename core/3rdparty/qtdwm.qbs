import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
    name: "Qtdwm"
    sourcePath: "qtdwm"

	Depends { name: "Qt"; submodules: [ 'core', 'gui' ] }
    Depends { name: "Qt.widgets"; condition: Qt.core.versionMajor === 5 }

    Export {
        Properties {
            condition: qbs.targetOS.contains("windows")
            Depends { name: "cpp" }
            cpp.dynamicLibraries: "gdi32"
        }
    }
}
