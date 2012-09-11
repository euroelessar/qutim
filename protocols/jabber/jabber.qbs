import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    Depends { name: "jreen" }
    Depends { name: "qca" }

    cpp.includePaths: ["src"]
}
