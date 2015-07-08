import "../Integration.qbs" as Integration

Integration {
    pluginTags: 'linux'

    Depends { condition: qbs.targetOS.contains("linux"); name: "Qt.dbus" }
}
