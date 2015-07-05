import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
	name: "flowlayout"
    sourcePath: "flowlayout"

	Depends { name: "Qt"; submodules: [ 'core', 'gui' ] }
    Depends { name: "Qt.widgets"; condition: qt.core.versionMajor === 5 }

    cpp.defines: [ "LIBQUTIM_STATIC" ]

    ProductModule {
        Depends { name: "cpp" }
        cpp.includePaths: product.path + "/flowlayout"
    }
}
