import "../../../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: false // change on true if need add to build
    sourcePath: '.'
    Depends { name: 'Qt.multimedia' }
}
