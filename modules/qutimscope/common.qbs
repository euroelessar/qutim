import qbs.base 1.0

Module {
    property string basePath
    property string shareDir: {
        if (qbs.targetOS === "mac")
            return "qutim.app/Contents/Resources/share";
        else if (qbs.targetOS === "linux")
            return "share/apps/qutim";
        else
            return "share";
    }
}
