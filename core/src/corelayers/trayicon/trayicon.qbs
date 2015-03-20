import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
	Group {
		name: "SystemNotifierItem"
		condition: qbs.targetOS.contains("linux")
		files: [
			"dbussrc/**/*.h",
			"dbussrc/**/*.cpp"
		]
	}
    Group {
        name: "QML files"
        files: "qml"
        qbs.install: true
        qbs.installDir: project.qutim_share_path
    }

    Depends {
        name: "Qt.dbus"
        condition: qbs.targetOS.contains("linux")
    }
}
