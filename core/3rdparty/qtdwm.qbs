import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
    name: "qtdwm"
    sourcePath: "qtdwm"

	Depends { name: "qt"; submodules: [ 'core', 'gui' ] }
    Depends { name: "qt.widgets"; condition: qt.core.versionMajor === 5 }

    ProductModule {
        Depends { name: "windows.gdi32"; condition: qbs.targetOS === 'windows' }
    }
}
