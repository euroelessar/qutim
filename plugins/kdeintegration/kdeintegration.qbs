import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    pluginTags: 'kde'
    Depends { name: "kde"; submodules: [ 'kio', 'knotifyconfig', 'kutils', 'core', 'ui', 'kemoticons' ]}
    cpp.includePaths: "."
}
