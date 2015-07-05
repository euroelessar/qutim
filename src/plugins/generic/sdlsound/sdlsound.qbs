import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    condition: qbs.targetOS.contains("linux")
    cpp.dynamicLibraries: [ "SDL_mixer", "SDL" ]
}
