import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    Depends { name: "otr" }

    //temporary hack
    Properties {
        condition: qbs.toolchain === "mingw"
        cpp.defines: ["OTR_INTERNAL"]
    }
}
