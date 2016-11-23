import qbs.base
import qbs.FileInfo
import qbs.File
import qbs.TextFile

Product {
    name: "share"

    type: [ "installed_content" ]

    property bool installConfig: true
    property bool installSoundTheme: true
    property bool installIcons: true
    property bool installOxygenTheme: !qbs.targetOS.contains("linux")
    property bool installUbuntuTheme: qbs.targetOS.contains("linux")
    property bool installHicolorTheme: !qbs.targetOS.contains("osx") && !qbs.targetOS.contains("windows")
    property bool installClientsIcons: true
    property string qutim_version: project.qutim_version

    Depends { name: "Qt.core" }
    
    property string shareDir: project.qutim_share_path

    Group {
        name: "Config files"
        condition: installConfig
        fileTags: "install"
        qbs.installDir: project.qutim_share_path + "/config"
        qbs.install: true
        prefix: {
            if (qbs.targetOS.contains("osx"))
                return "../../config/mac/";
            else if (qbs.targetOS.contains("windows"))
                return "../../config/win/";
            else
                return "../../config/generic/";
        }
        files: "*.json"
    }

    Group {
        name: "CA certificates"
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/ca-certs/"
        qbs.install: true
        files: "qutim/ca-certs/**"
    }

    Group {
        name: "WebKit Styles"
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/webkitstyle/"
        qbs.install: true
        files: "qutim/webkitstyle/**"
        qbs.installSourceBase: "qutim/webkitstyle"
    }

    Group {
        name: "Sounds"
        condition: installSoundTheme
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/sounds/"
        qbs.install: true
        files: "../../artwork/sounds/**"
        qbs.installSourceBase: "../../artwork/sounds"
    }

    Group {
        name: "Desktop file"
        condition: qbs.targetOS.contains("linux")
        fileTags: [ "install" ]
        qbs.installDir: "share/applications"
        qbs.install: true
        prefix: "../../src/share/applications/"
        files: "**"
    }

    Group {
        name: "Default tray icons"
        condition: installIcons && qbs.targetOS.contains("linux")
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_system_share_path
        qbs.install: true
        files: "../../artwork/icons/qutim/**"
        qbs.installSourceBase: "../../artwork/icons/qutim/"
    }
    
    Group {
        name: "Default icon theme"
        condition: installIcons
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/icons/qutim-default/"
        qbs.install: true
        files: "../../artwork/icons/qutim-default/**"
        qbs.installSourceBase: "../../artwork/icons/qutim-default"
    }

    Group {
        name: "System fallback theme"
        condition: installIcons && installHicolorTheme && !qbs.targetOS.contains("linux")
        // thanks qbs for not allowing to install one file to two destinations
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/icons/hicolor/"
        qbs.install: true
        files: ["../../artwork/icons/qutim/icons/hicolor/**"]
        qbs.installSourceBase: "../../artwork/icons/qutim/icons/hicolor/"
    }

    Group {
        name: "User-status icons"
        condition: installIcons
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/icons/qutim-default/user-status/"
        qbs.install: true
        prefix: "../../artwork/icons/user-status/**"
        qbs.installSourceBase: "../../artwork/icons/user-status"
    }

    Group {
        name: "User-role icons"
        condition: installIcons
        fileTags: [ "install" ]
        qbs.installDir: shareDir + "/icons/hicolor/scalable/status/"
        qbs.install: true
        files: "../../artwork/icons/user-status/hicolor/scalable/status/*.svg"
    }

    Group {
        name: "Clients icons"
        condition: installIcons && installClientsIcons
        fileTags: [ "install" ]
        qbs.installDir: shareDir + "/icons/hicolor/16x16/apps/"
        qbs.install: true
        files: "../../artwork/icons/clients/hicolor/16x16/apps/*"
        excludeFiles: "../../artwork/icons/clients/hicolor/16x16/apps/qutim.png"
    }

    Group { // YES, this will install Humanity (case matters!) as hicolor
        name: "Humanity icons"
        condition: installIcons && qbs.targetOS.contains("linux")
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/icons/hicolor/"
        qbs.install: true
        files: "../../artwork/icons/humanity/hicolor/**"
        qbs.installSourceBase: "../../artwork/icons/humanity/hicolor/"
    }

    Group { // Mac tray icons
        name: "TrayIcon [osx]"
        condition: installIcons && qbs.targetOS.contains("osx")
        fileTags: [ "install" ]
        qbs.install: true
        qbs.installDir: project.qutim_share_path + "/icons/qutim-default/scalable/status/"
        files: "../../artwork/icons/tray/MacOS/*.svg"
    }

    Group {
        name: "Oxygen icons"
        condition: installIcons && installOxygenTheme
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/icons/oxygen/"
        qbs.install: true
        files: "../../artwork/icons/oxygen/**"
        qbs.installSourceBase: "../../artwork/icons/oxygen"
    }

    Group {
        name: "Ubuntu mono dark icons"
        condition: installIcons && installUbuntuTheme
        fileTags: [ "install" ]
        qbs.install: true
        qbs.installDir: shareDir + "/icons/ubuntu-mono-light/scalable/status/"
        files: "../../artwork/icons/tray/ubuntu-mono-light/*.svg"
    }

    Group {
        name: "Ubuntu mono light icons"
        condition: installIcons && installUbuntuTheme
        fileTags: [ "install" ]
        qbs.installDir: shareDir + "/icons/ubuntu-mono-dark/scalable/status/"
        qbs.install: true
        files: "../../artwork/icons/tray/ubuntu-mono-dark/*.svg"
    }

    Group {
        name: "Dock icon"
        condition: qbs.targetOS.contains("osx")
        fileTags: [ "install" ]
        qbs.install: true
        qbs.installDir: "bin/qutim.app/Contents/Resources"
        files: ["qutim.icns"]
    }

    Group {
        name: "QtMenu.nib [osx]"
        condition: qbs.targetOS.contains("osx")
        fileTags: [ "install" ]
        qbs.install: true
        qbs.installDir: "bin/qutim.app/Contents/Resources/qt_menu.nib/"
        prefix: Qt.core.libPath + '/QtGui' + Qt.core.libInfix + '.framework/Versions/' + Qt.core.versionMajor + '/Resources/qt_menu.nib/'
        files: '*.nib'
    }
}
