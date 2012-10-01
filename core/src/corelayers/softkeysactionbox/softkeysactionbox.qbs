import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: qbs.targetOS === "symbian"
    sourcePath: ''
}
