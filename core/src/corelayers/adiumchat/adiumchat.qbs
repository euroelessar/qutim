import qbs.base 1.0
import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: "chatlayer"
    cpp.defines: base.concat('ADIUMCHAT_LIBRARY')

    //Depends { name: "flowlayout" }
    Depends { name: "adiumchat-headers" }
    ProductModule {
		Depends { name: "cpp" }
        Depends { name: "adiumchat-headers" }
        cpp.includePaths: [
			product.buildDirectory
		]
	}
}
