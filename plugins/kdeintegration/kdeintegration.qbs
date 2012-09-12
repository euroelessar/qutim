import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    Depends { name: "kde"; submodules: [ 'io', 'notifyconfig', 'utils' ]}
    cpp.includePaths: "."
}
