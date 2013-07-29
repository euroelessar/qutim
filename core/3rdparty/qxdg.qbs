import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
	name: "qxdg"
    sourcePath: "q-xdg/src"

    Depends { name: "Qt"; submodules: [ 'core', 'gui', 'widgets' ] }

    cpp.defines: [ "XDG_STATIC" ]
	Export {
		cpp.defines: [ "XDG_STATIC" ]
	}
}
