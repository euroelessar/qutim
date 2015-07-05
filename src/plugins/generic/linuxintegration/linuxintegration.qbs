import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    condition: qbs.targetOS.contains("linux")
    sourcePath: ""
}
