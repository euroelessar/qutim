import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
	name: "qxdg"

    Depends { name: "qt"; submodules: [ 'core' ] }
	
	files: [
		"q-xdg/src/xdgenvironment.cpp",
		"q-xdg/src/xdgenvironment.h",
		"q-xdg/src/xdgexport.h",
		"q-xdg/src/xdg.h",
		"q-xdg/src/xdgicon.cpp",
		"q-xdg/src/xdgiconengine.cpp",
		"q-xdg/src/xdgiconengine_p.h",
		"q-xdg/src/xdgicon.h",
		"q-xdg/src/xdgiconmanager.cpp",
		"q-xdg/src/xdgiconmanager.h",
		"q-xdg/src/xdgiconmanager_p.h",
		"q-xdg/src/xdgicontheme.cpp",
		"q-xdg/src/xdgicontheme.h",
		"q-xdg/src/xdgicontheme_p.h",
		"q-xdg/src/xdgthemechooser.cpp",
		"q-xdg/src/xdgthemechooser.h"
	]
    cpp.defines: [ "XDG_STATIC" ]
}
