import qbs.base 1.0
import "../../../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    sourcePath: ""

    Depends { name: "qutim-adiumchat" }
    Depends { name: "x11"; condition: qbs.targetOS === 'linux' }
}
