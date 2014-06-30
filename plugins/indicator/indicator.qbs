import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    cpp.dynamicLibraries: ["messaging-menu-qt5"]
    condition: qbs.targetOS.contains("linux")
}
