import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: false //specific plugin for specific usage
    Depends { name: "qca" }
}
