import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: 'src'

    Depends { name: "hmac" }
}
