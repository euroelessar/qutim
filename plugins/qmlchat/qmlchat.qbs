import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: '.'

    Depends { name: "qt.quick1" }
    Depends { name: "qt.opengl" }
    Depends { name: "adiumchat" }
}
