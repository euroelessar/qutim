import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    Depends { name: "jreen" }

    cpp.includePaths: ["src"]
}
