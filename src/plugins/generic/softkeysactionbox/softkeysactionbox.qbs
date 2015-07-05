import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    condition: qbs.targetOS === "symbian"
    sourcePath: ''
}
