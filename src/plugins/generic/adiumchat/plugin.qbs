import qbs.base 1.0
import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    name: "adiumchat"
    sourcePath: "chatlayer"

    //Depends { name: "flowlayout" }
    Depends { name: "qutim-adiumchat" }
}
