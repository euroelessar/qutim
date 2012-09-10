import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
	name: "slidingstackedwidget"
    sourcePath: "slidingstackedwidget"

	Depends { name: "qt"; submodules: [ 'core', 'gui' ] }
    Depends { name: "qt.widgets"; condition: qt.core.versionMajor === 5 }

    cpp.defines: [ "LIBQUTIM_LIBRARY" ]

    ProductModule {
        Depends { name: "cpp" }
        cpp.includePaths: product.path + "/slidingstackedwidget"
    }
}
