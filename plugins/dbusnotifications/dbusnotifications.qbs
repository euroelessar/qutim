import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    pluginTags: 'linux'

    Depends { condition: qbs.targetOS.contains("linux"); name: "Qt.dbus" }
}
