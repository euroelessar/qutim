import "../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: 'xstatus'
    name: "xstatus"

    Depends { name: "oscar" }
}
