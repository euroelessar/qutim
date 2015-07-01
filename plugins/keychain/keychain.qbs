import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    //Depends { name: "qt5keychain" }
    cpp.dynamicLibraries: ["qt5keychain"]
}
