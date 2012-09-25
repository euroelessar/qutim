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
    property var pluginTags: ['core', 'desktop']

    Properties {
        condition: qbs.targetOS === 'linux'
        pluginTags: outer.concat(['linux'])
    }
    Properties {
        condition: qbs.targetOS === 'windows'
        pluginTags: outer.concat('windows')
    }
    Properties {
        condition: qbs.targetOS === 'mac'
        pluginTags: outer.concat('mac')
    }
}
