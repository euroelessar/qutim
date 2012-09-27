import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    pluginTags: 'linux'
    sourcePath: 'src'

    cpp.defines: [
        'PURPLE_LIBDIR="/usr/lib/"' //I want to use purple.libraryPaths
    ]

    Depends { name: "purple" }
    Depends { name: "glib" }
}
