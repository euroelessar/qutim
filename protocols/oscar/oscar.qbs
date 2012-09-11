import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    property bool useThirdPartyHmac: true

    cpp.defines: base.concat([ "LIBOSCAR_LIBRARY" ])

    Depends { name: "qca"; condition: !useThirdPartyHmac }

    Properties {
        condition: !useThirdPartyHmac
        cpp.defines: outer.concat([ "OSCAR_USE_QCA2" ])
    }
    Properties {
        condition: useThirdPartyHmac
        cpp.defines: outer.concat([ "OSCAR_USE_3RDPARTY_HMAC" ])
    }

    //hmac
    Group {
        condition: useThirdPartyHmac
        prefix: "3rdparty/hmac/"
        files: [ "*.c", "*.h" ]
    }
}
