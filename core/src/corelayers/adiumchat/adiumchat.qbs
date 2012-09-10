import qbs.base 1.0
import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: "chatlayer"

    Depends { name: "flowlayout" }
    Depends { name: "adiumchat-headers" }
	ProductModule {
		Depends { name: "cpp" }
        Depends { name: "adiumchat-headers" }
        cpp.includePaths: [
			product.buildDirectory
		]
	}
}
