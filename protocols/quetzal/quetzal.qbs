import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: 'src'

    cpp.defines: ["DPURPLE_LIBDIR=" + "/usr/lib"] //in PkgConfigProbe i cannot determine purple libdir((((

    Depends { name: "purple" }
    Depends { name: "glib" }
}
