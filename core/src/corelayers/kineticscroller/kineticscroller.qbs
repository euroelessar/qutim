import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ''
	cpp.includePaths: '.'

	Depends { name: "qt.webkit"; condition: qbs.targetOS !== "symbian" }
}
