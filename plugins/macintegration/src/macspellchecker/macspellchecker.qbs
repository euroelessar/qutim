import "../../../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: qbs.targetOS === "mac"
    sourcePath: ""

    Depends { name: "cocoa" }
}
