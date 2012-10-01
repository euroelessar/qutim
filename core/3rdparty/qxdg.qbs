import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
	name: "qxdg"
    sourcePath: "q-xdg/src"

    Depends { name: "qt"; submodules: [ 'core' ] }

    cpp.defines: [ "XDG_STATIC" ]
	ProductModule {
		Depends { name: "cpp" }
		cpp.defines: [ "XDG_STATIC" ]
	}
}
