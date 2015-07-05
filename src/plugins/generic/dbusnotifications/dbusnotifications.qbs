import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    pluginTags: 'linux'

    Depends { condition: qbs.targetOS.contains("linux"); name: "Qt.dbus" }
}
