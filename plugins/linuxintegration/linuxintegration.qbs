import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: qbs.targetOS === "linux"
    sourcePath: ""
}
