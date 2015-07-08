import "../Integration.qbs" as Integration

Integration {
    pluginTags: 'linux'
    Depends { name: "kde"; submodules: [ 'kio', 'knotifyconfig', 'kutils', 'core', 'ui', 'kemoticons' ]}
    cpp.includePaths: "."
}
