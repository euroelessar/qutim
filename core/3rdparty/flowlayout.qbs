import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
	name: "flowlayout"

	Depends { name: "qt"; submodules: [ 'core', 'gui' ] }

	files: [
		"flowlayout/flowlayout.cpp",
		"flowlayout/flowlayout.h"
	]
}
