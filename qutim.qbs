import qbs.base 1.0

Project {
    name: "qutIM"

    property string qutim_version_major: '0'
    property string qutim_version_minor: '3'
    property string qutim_version_release: '2'
    property string qutim_version_patch: '0'
    property string qutim_version: qutim_version_major + '.' + qutim_version_minor + '.' + qutim_version_release + '.' + qutim_version_patch
    property bool declarativeUi: false
    property var additionalCppDefines: []
    property string shareDir: {
        if (qbs.targetOS.contains("mac"))
            return "qutim.app/Contents/Resources/share";
        else if (qbs.targetOS.contains("linux"))
            return "share/apps/qutim";
        else
            return "share";
    }
    property var pluginTags: {
        var tags = [ "desktop", "core" ];
        return tags.concat(qbs.targetOS);
    }

    property bool singleProfile: true
    moduleSearchPaths: "modules"

    references: [
        "core/3rdparty/3rdparty.qbs",
        "core/core.qbs",
        "core/src/corelayers/corelayers.qbs",
        "plugins/plugins.qbs",
        "protocols/protocols.qbs"
    ]
}

