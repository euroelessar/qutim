import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ''
	cpp.includePaths: '.'
        
	Depends { name: "qt.webkit"; condition: qbs.targetOS !== "symbian" }
    Properties {
        condition: qbs.targetOS === "symbian"
        cpp.defines: base.concat([ "QTSCROLLER_NO_WEBKIT" ])
    }
}
