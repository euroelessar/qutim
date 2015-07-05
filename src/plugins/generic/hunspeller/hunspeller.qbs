import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    pluginTags: 'linux'
    cpp.dynamicLibraries: ["hunspell"]
    condition: qbs.targetOS.contains("linux")
}
