import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    Depends { name: "aspell" }
//    condition: aspell.found // Doesn't work for now
}
