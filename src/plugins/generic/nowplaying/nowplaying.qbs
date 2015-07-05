import "../GenericPlugin.qbs" as GenericPlugin
import qbs.fileinfo 1.0 as FileInfo

GenericPlugin {
    pluginTags: 'linux'

    sourcePath: '.'
    Depends { name: 'qt.dbus' }

    cpp.includePaths: [
        'include',
        'src',
        product.path, //plugin is really crap
    ]
}
