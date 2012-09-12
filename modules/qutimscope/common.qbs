import qbs.base 1.0

Module {
    property string basePath
    property string shareDir: {
    if (qbs.targetOS === "mac")
        return "qutim.app/Contents/Resources/share"
    else
        return "share"
    }

}
