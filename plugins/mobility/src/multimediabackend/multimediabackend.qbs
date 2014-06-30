import "../../../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: qbs.targetOS.contains("windows")
    sourcePath: '.'
    Depends { name: 'Qt.multimedia' }
}
