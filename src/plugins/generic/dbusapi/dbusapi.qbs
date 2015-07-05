import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    condition: qbs.targetOS === "linux"
    Depends { name: "Qt.dbus" }
}
