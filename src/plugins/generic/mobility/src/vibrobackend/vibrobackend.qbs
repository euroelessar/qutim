import "../GenericPlugin.qbs" as GenericPlugin

GenericPlugin {
    sourcePath: '.'
    Depends { name: 'qt.feedback' }
}
