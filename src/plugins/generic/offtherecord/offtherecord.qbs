import "../GenericPlugin.qbs" as GenericPlugin
import qbs.Probes

GenericPlugin {
    Probes.PkgConfigProbe {
        id: pkgConfig
        name: "libotr"
        minVersion: "4.0.0"
    }
    
    condition: pkgConfig.found
    pluginTags: ['linux', 'crypto', 'mac']
    
    cpp.includePaths: pkgConfig.includePaths
    cpp.libraryPaths: pkgConfig.libraryPaths
    cpp.dynamicLibraries: pkgConfig.dynamicLibraries
    cpp.linkerFlags: pkgConfig.linkerFlags
}
