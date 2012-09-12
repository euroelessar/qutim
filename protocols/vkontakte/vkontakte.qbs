import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    Depends { name: "vreen" }
    Depends { name: "vreenoauth" }
    Depends { name: "qt.quick1" }
    Depends { name: "qt.webkit" }

    cpp.includePaths: ["src"]
}
