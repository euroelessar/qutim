import qbs.base 1.0
import "../../../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ""

    Depends { name: "qutim-adiumchat" }
    Depends { name: "x11"; condition: qbs.targetOS === 'linux' }
}
