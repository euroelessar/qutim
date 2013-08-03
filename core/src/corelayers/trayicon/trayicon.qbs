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

    Depends {
        name: "Qt.dbus"
        condition: qbs.targetOS.contains("linux")
    }
}
