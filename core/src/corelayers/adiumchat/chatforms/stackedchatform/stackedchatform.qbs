import qbs.base 1.0
import "../../../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ""

	Depends { name: "adiumchat" }
//    Depends { name: "slidingstackedwidget" }
    Depends { name: "x11" }
}
