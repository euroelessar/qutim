import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    pluginTags: 'linux'
    Depends { name: "kde"; submodules: [ 'kio', 'knotifyconfig', 'kutils', 'core', 'ui', 'kemoticons' ]}
    cpp.includePaths: "."
}
