import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: qbs.targetOS === "linux"
    Depends { name: "Qt.dbus" }
}
