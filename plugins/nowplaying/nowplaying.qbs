import "../UreenPlugin.qbs" as UreenPlugin
import qbs.fileinfo 1.0 as FileInfo

UreenPlugin {
    condition: qbs.targetOS === 'linux'

    sourcePath: '.'
    Depends { name: 'qt.dbus' }

    cpp.includePaths: [
        'include',
        'src',
        product.path, //plugin is really crap
    ]
}
