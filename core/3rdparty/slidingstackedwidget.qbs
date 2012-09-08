import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
	name: "slidingstackedwidget"

	Depends { name: "qt"; submodules: [ 'core', 'gui' ] }
    Depends { name: "qt.widgets"; condition: qt.core.versionMajor === 5 }
	
	files: [
		"slidingstackedwidget/fingerswipegesture.cpp",
		"slidingstackedwidget/fingerswipegesture.h",
		"slidingstackedwidget/slidingstackedwidget.cpp",
		"slidingstackedwidget/slidingstackedwidget.h"
	]
}
