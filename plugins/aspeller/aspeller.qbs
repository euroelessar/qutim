import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    pluginTags: 'linux'
    Depends { name: "aspell" }
//    condition: aspell.found // Doesn't work for now
}
