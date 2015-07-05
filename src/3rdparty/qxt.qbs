import qbs.base 1.0
import "./3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
    name: "qxt"
    sourcePath: "qxt"

    Depends { name: "Qt"; submodules: [ 'core' ] }

    cpp.defines: [ "QXT_STATIC" ]

    Export {
        cpp.includePaths: [
            "qxt"
        ]
        cpp.defines: [ "NO_SYSTEM_QXT", "QXT_STATIC" ]
    }
}
