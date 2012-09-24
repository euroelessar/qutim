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
    property var pluginTags: {
        var tags = [ 'core' ];
        if (qbs.targetOS === 'mac')
            tags.concat('mac');
        if (qbs.targetOS === 'linux')
            tags.concat('linux', 'kde');
        if (qbs.targetOS === 'windows')
            tags.concat('windows');
        tags.concat('desktop');
        return tags;
    }
}
