import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ''
	cpp.includePaths: '.'
        
	Depends {
        condition: !qbs.targetOS.contains("symbian")
        name: "Qt"
        submodules: [ "webkit", "webkitwidgets" ]
    }
    Properties {
        condition: qbs.targetOS === "symbian"
        cpp.defines: base.concat([ "QTSCROLLER_NO_WEBKIT" ])
    }
}
