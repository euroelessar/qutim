import "../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: 'xstatus'
    name: "oscarxstatus"

    Depends { name: "oscar" }
}
