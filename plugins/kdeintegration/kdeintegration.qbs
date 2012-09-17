import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    Depends { name: "kde"; submodules: [ 'kio', 'knotifyconfig', 'kutils', 'core', 'ui', 'kemoticons' ]}
    cpp.includePaths: "."
}
