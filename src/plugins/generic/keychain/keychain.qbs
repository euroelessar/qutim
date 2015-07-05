import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    //Depends { name: "qt5keychain" }
    cpp.dynamicLibraries: ["qt5keychain"]
}
