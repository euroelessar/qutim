import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    condition: false //qbs.targetOS === 'linux'

    Depends { name: "telepathy.qt" }
}
