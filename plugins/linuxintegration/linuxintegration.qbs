import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: qbs.targetOS.contains("linux")
    sourcePath: ""
}
