import qbs.base 1.0
import "../../../core/3rdparty/3rdPartyLibrary.qbs" as ThirdPartyLibrary

ThirdPartyLibrary {
    name: "hmac"
    sourcePath: "hmac"

    ProductModule {
        Depends { name: "cpp" }
        cpp.defines: "OSCAR_USE_3RDPARTY_HMAC"
    }
}
