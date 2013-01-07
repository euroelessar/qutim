import "../../../../core/Framework.qbs" as Framework

Framework {
    name: "qutim-adiumchat"
    type: ["dynamiclibrary", "installed_content"]

    Depends { name: "cpp" }
    Depends { name: "libqutim" }
    Depends { name: "qt"; submodules: [ "core", "gui" ] }
    Depends { name: "qt.widgets"; condition: qt.core.versionMajor === 5 }

    cpp.defines: "ADIUMCHAT_LIBRARY"
    cpp.visibility: 'hidden'

    files: "lib/*.cpp"

    Group {
        qbs.installDir: "include/qutim/adiumchat"
        fileTags: ["install"]
        overrideTags: false
        files: [
            "lib/*.h"
        ]
    }
}
