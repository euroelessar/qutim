import "../Integration.qbs" as Integration

Integration {
    cpp.dynamicLibraries: ["messaging-menu-qt5"]
    condition: qbs.targetOS.contains("linux")
}
