import "../Integration.qbs" as Integration

Integration {
    condition: qbs.targetOS.contains("linux")
    sourcePath: ""
}
