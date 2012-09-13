import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: qbs.targetOS === "linux"
    Depends { name: "qt.dbus" }

    Group {
        fileTags: 'qdbusxml'
        files: 'org.freedesktop.Notifications.xml'
    }
}
