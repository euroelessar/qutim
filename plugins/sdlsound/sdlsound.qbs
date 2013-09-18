import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: qbs.targetOS.contains("linux")
    cpp.dynamicLibraries: [ "SDL_mixer", "SDL" ]
}
