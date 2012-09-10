import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: 'src_' //hack, group found src recursive in ./src, vreen/src, vreen/oauth/src, etc...

    Depends { name: "vreen" }
    Depends { name: "vreenoauth" }
    Depends { name: "qt.quick1" }
    Depends { name: "qt.webkit" }

    cpp.includePaths: ["src", "src/experimental", "src/ui"]

    files: [
        "src/*.h",
        "src/*.cpp",
        "src/experimental/*.h",
        "src/experimental/*.cpp",
        "src/ui/*.h",
        "src/ui/*.cpp",
        "src/ui/*.ui",
    ]
}
