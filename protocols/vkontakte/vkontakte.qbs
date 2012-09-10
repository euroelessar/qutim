import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: 'src'

    Depends { name: "vreen" }
    Depends { name: "vreenoauth" }
    Depends { name: "qt.quick1" }
}
