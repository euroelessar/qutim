import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
    name: "qxt"

    Depends { name: "qt"; submodules: [ 'core' ] }

    files: [
        "qxt/qxtcommandoptions.h",
        "qxt/qxtglobal.h",
        "qxt/qxtcommandoptions.cpp",
    ]
    cpp.defines: [ "QXT_STATIC" ]

    ProductModule {
        Depends { name: "cpp" }
        cpp.includePaths: [
            "qxt"
        ]
        cpp.defines: [ "NO_SYSTEM_QXT" ]
    }
}
