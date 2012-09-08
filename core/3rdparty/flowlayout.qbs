import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
	name: "flowlayout"

	Depends { name: "qt"; submodules: [ 'core', 'gui' ] }
    Depends { name: "qt.widgets"; condition: qt.core.versionMajor === 5 }

	files: [
		"flowlayout/flowlayout.cpp",
		"flowlayout/flowlayout.h"
	]
    cpp.defines: [ "LIBQUTIM_LIBRARY" ]
}
