import "../Integration.qbs" as Integration

Integration {
    condition: qbs.targetOS === "symbian"
    sourcePath: ''
}
