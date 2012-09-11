import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: qbs.targetOS === 'linux'
    sourcePath: 'src'

    cpp.defines: ["DPURPLE_LIBDIR=" + "/usr/lib"] //in PkgConfigProbe i cannot determine purple libdir((((

    Depends { name: "purple" }
    Depends { name: "glib" }
}
