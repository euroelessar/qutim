import "../Integration.qbs" as Integration

Integration {
    condition: qbs.targetOS === "linux"
    Depends { name: "Qt.dbus" }
}
