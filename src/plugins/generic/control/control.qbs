import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    condition: false //specific plugin for specific usage
    Depends { name: "qca" }
}
