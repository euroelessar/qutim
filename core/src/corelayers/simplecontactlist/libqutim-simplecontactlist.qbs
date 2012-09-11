import qbs.base 1.0
import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

DynamicLibrary {
    name: "libqutim-simplecontactlist"

    Depends { name: "cpp" }
    Depends { name: "libqutim" }
    Depends { name: "qt"; submodules: [ "core", "gui" ] }
    Depends { name: "simplecontactlist-headers" }

    cpp.dynamicLibraryPrefix: ""
    cpp.defines: "SIMPLECONTACTLIST_LIBRARY"
    destination: "lib"

    Group {
        files: [
            "lib/*.h",
            "lib/*.cpp",
            "lib/*.ui"
        ]
    }

    ProductModule {
        Depends { name: "simplecontactlist-headers" }
    }
}
