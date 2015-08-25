import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    pluginTags: 'linux'

    sourcePath: '.'
    Depends { name: 'Qt.dbus' }

    cpp.includePaths: [
        'include',
        'src',
    ]
}
