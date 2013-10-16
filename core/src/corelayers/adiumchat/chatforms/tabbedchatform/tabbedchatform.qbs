import qbs.base 1.0
import "../../../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ""

    Depends { name: "qutim-adiumchat" }
    Properties {
        condition: qbs.targetOS.contains('linux')
        cpp.dynamicLibraries: ["X11","Qt5X11Extras" ]
    }
}
