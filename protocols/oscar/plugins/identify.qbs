import "../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: 'identify'
    name: "identify"

    Depends { name: "oscar" }
}
