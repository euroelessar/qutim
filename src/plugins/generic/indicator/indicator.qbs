import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    cpp.dynamicLibraries: ["messaging-menu-qt5"]
    condition: qbs.targetOS.contains("linux")
}
