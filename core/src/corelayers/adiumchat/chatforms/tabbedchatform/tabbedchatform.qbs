import qbs.base 1.0
import "../../../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: ""

    Depends { name: "adiumchat" }
    Depends { name: "x11"; condition: qbs.targetOS === 'linux' }
}
