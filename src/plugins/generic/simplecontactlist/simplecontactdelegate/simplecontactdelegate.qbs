import qbs.base 1.0
import "../../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
	name: "simplecontactdelegate"
    sourcePath: ""

    Depends { name: "qutim-simplecontactlist" }
}
