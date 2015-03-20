import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    pluginTags: 'linux'
    cpp.dynamicLibraries: ["hunspell"]
    condition: qbs.targetOS.contains("linux")
}
