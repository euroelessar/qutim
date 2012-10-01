import "../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: 'identify'
    name: "oscaridentify"

    Depends { name: "oscar" }
}
