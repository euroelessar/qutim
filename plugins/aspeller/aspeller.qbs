import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    pluginTags: 'linux'
    cpp.dynamicLibraries: ["aspell"]
    condition: qbs.targetOS.contains("linux")

    //Depends { name: "aspell" }
//    condition: aspell.found // Doesn't work for now
}
