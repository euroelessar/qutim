import qbs.base 1.0
import qbs.fileinfo as FileInfo

Product {
    type: "installed_content"

    property bool installConfig: true
    property bool installSoundTheme: true
    property bool installIcons: true
    property bool installOxygenTheme: true
    property bool installUbuntuTheme: true

    Depends { name: "qutimscope" }
    Depends { name: "artwork" }
    
    property string shareDir: qutimscope.shareDir

    Group {
        condition: installConfig
        fileTags: "install"
        qbs.installDir: shareDir + "/config"
        prefix: {
            if (qbs.targetOS === "mac")
                return "../config/mac/";
            else if (qbs.targetOS === "windows")
                return "../config/win/";
            else
                return "../config/generic/";
        }
        files: "*.json"
    }

    Group {
        fileTags: [ "artwork" ]
        qutimscope.basePath: "share"
        prefix: "share/qutim/"
        recursive: true
        files: "*"
    }

    Group {
        condition: installSoundTheme
        fileTags: [ "artwork" ]
        qutimscope.basePath: "../artwork"
        prefix: "../artwork/sounds/"
        recursive: true
        files: "*"
    }

    Group {
        condition: installIcons
        fileTags: [ "artwork" ]
        qutimscope.basePath: "../artwork"
        prefix: "../artwork/icons/qutim-default/"
        recursive: true
        files: "*"
    }

    Group {
        condition: installIcons
        fileTags: [ "artwork" ]
        qutimscope.basePath: "../artwork"
        prefix: "../artwork/icons/user-status/hicolor/"
        recursive: true
        files: "*"
    }

    Group {
        condition: installIcons && qbs.targetOS === "linux"
        fileTags: [ "artwork" ]
        qutimscope.basePath: "../artwork"
        prefix: "../artwork/icons/humanity/hicolor/"
        recursive: true
        files: "*"
    }

    Group {
        condition: installIcons && qbs.targetOS === "mac"
        fileTags: [ "install" ]
        qbs.installDir: shareDir + "/icons/qutim-default/scalable/status/"
        files: "../artwork/icons/tray/MacOS/*.svg"
    }

    Group {
        condition: installIcons && installOxygenTheme
        fileTags: [ "artwork" ]
        qutimscope.basePath: "../artwork"
        prefix: "../artwork/icons/oxygen/"
        recursive: true
        files: "*"
    }

    Group {
        condition: installIcons && installUbuntuTheme
        fileTags: [ "install" ]
        qbs.installDir: shareDir + "/icons/ubuntu-mono-light/scalable/status/"
        files: "../artwork/icons/tray/ubuntu-mono-light/*.svg"
    }

    Group {
        condition: installIcons && installUbuntuTheme
        fileTags: [ "install" ]
        qbs.installDir: shareDir + "/icons/ubuntu-mono-dark/scalable/status/"
        files: "../artwork/icons/tray/ubuntu-mono-dark/*.svg"
    }

}
