import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    property bool useThirdPartyHmac: true

    cpp.defines: base.concat([ "LIBOSCAR_LIBRARY" ])
    sourcePath: 'src'

    Depends { name: "qca"; condition: !useThirdPartyHmac }
    Depends { name: "hmac"; condition: useThirdPartyHmac }

    Properties {
        condition: !useThirdPartyHmac
        cpp.defines: outer.concat("OSCAR_USE_QCA2")
    }
}
