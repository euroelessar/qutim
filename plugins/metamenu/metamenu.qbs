import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: qbs.getenv("XDG_CURRENT_DESKTOP") === "Unity"
}
