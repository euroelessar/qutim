import qbs.base 1.0
import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    name: "adiumchat"
    sourcePath: "chatlayer"

    //Depends { name: "flowlayout" }
    Depends { name: "qutim-adiumchat" }
}
