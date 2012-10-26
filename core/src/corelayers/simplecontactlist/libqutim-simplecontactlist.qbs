import "../../../../core/Framework.qbs" as Framework

Framework {
    name: "qutim-simplecontactlist"

    Depends { name: "cpp" }
    Depends { name: "libqutim" }
    Depends { name: "qt"; submodules: [ "core", "gui" ] }
    Depends { name: "simplecontactlist-headers" }

    cpp.defines: "SIMPLECONTACTLIST_LIBRARY"
    cpp.visibility: 'hidden'

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
