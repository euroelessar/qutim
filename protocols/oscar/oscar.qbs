import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    property bool useThirdPartyHmac: true

    cpp.defines: base.concat([ "LIBOSCAR_LIBRARY" ])
}
