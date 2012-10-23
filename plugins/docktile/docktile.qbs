import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    pluginTags: [ 'linux', 'windows' ]

    Depends { name: "qtdocktile" }
}
