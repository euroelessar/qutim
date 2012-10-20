import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    pluginTags: ['linux', 'crypto', 'mac']

    Depends { name: "otr" }    
}
