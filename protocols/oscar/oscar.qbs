import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    property bool useThirdPartyHmac: true

    cpp.defines: base.concat([ "LIBOSCAR_LIBRARY" ])

    Depends {
        name: "qca"
        condition: qt.core.versionMajor < 5 && !useThirdPartyHmac
    }
    Depends {
        name: "hmac"
        condition: qt.core.versionMajor < 5 && useThirdPartyHmac
    }

    Properties {
        condition: qt.core.versionMajor < 5 && !useThirdPartyHmac
        cpp.defines: outer.concat("OSCAR_USE_QCA2")
    }
}
