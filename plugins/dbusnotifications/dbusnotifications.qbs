import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    Depends { name: "qt.dbus" }

    Group {
        fileTags: 'qdbusxml'
        files: 'org.freedesktop.Notifications.xml'
    }
}
