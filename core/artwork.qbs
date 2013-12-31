import qbs.base
import qbs.FileInfo
import qbs.File
import qbs.TextFile

Product {
    name: "nameless-artwork"

    type: [" installed_content" ]

    property bool installConfig: true
    property bool installSoundTheme: true
    property bool installIcons: true
    property bool installOxygenTheme: !qbs.targetOS.contains("linux")
    property bool installUbuntuTheme: qbs.targetOS.contains("linux")
    property string qutim_version: project.qutim_version

    Depends { name: "Qt.core" }
    Depends { name: "artwork" }
    
    property string shareDir: artwork.shareDir

    Group {
        name: "config files"
        condition: installConfig
        fileTags: "install"
        qbs.installDir: project.qutim_share_path + "/config"
        qbs.install: true
        prefix: {
            if (qbs.targetOS.contains("osx"))
                return "../config/mac/";
            else if (qbs.targetOS.contains("windows"))
                return "../config/win/";
            else
                return "../config/generic/";
        }
        files: "*.json"
    }

    Group {
        name: "webkitstyle and ca-certs"
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path
        qbs.install: true
        files: "share/qutim/"
    }

    Group {
        name: "sounds"
        condition: installSoundTheme
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path
        qbs.install: true
        files: "../artwork/sounds"
    }

    Group {
        name: "desktop file"
        condition: qbs.targetOS.contains("linux")
        fileTags: [ "install" ]
        qbs.installDir: "share/applications"
        qbs.install: true
        prefix: "share/applications/"
        files: "**"
    }

    Group {
        name: "default tray icons"
        condition: installIcons && qbs.targetOS.contains("linux")
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/icons/"
        files: "../artwork/icons/qutim"
    }
    
    Group {
        name: "default icon theme"
        condition: installIcons
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/icons/"
        qbs.install: true
        files: "../artwork/icons/qutim-default"
    }

    Group {
        name: "user-status icons"
        condition: installIcons
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/icons/qutim-default/"
        qbs.install: true
        prefix: "../artwork/icons/user-status"
    }

    Group {
        name: "Humanity icons"
        condition: installIcons && qbs.targetOS.contains("linux")
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/icons/"
        qbs.install: true
        files: "../artwork/icons/humanity/"
    }

    Group { // Mac tray icons
        name: "TrayIcon [osx]"
        condition: installIcons && qbs.targetOS.contains("osx")
        fileTags: [ "install" ]
        qbs.install: true
        qbs.installDir: project.qutim_share_path + "/icons/qutim-default/scalable/status/"
        files: "../artwork/icons/tray/MacOS/*.svg"
    }

    Group {
        name: "Oxygen icons"
        condition: installIcons && installOxygenTheme
        fileTags: [ "install" ]
        qbs.installDir: project.qutim_share_path + "/icons"
        qbs.install: true
        files: "../artwork/icons/oxygen"
    }

    Group {
        name: "Ubuntu mono dark icons"
        condition: installIcons && installUbuntuTheme
        fileTags: [ "install" ]
        qbs.install: true
        qbs.installDir: shareDir + "/icons/ubuntu-mono-light/scalable/status/"
        files: "../artwork/icons/tray/ubuntu-mono-light/*.svg"
    }

    Group {
        name: "Ubuntu mono light icons"
        condition: installIcons && installUbuntuTheme
        fileTags: [ "install" ]
        qbs.installDir: shareDir + "/icons/ubuntu-mono-dark/scalable/status/"
        qbs.install: true
        files: "../artwork/icons/tray/ubuntu-mono-dark/*.svg"
    }

    Group {
        name: "Dock icon"
        condition: qbs.targetOS.contains("osx")
        fileTags: [ "install" ]
        qbs.install: true
        qbs.installDir: "bin/qutim.app/Contents/Resources"
        files: ["qutim.icns" /*, "qt.conf"*/]
    }

    Group {
        condition: qbs.targetOS.contains("osx")
        fileTags: [ "infoPlist" ]
        files: "Info.plist"
    }

    Group {
        condition: qbs.targetOS.contains("osx")
        fileTags: [ "install" ]
        qbs.install: true
        qbs.installDir: "bin/qutim.app/Contents/Resources/qt_menu.nib/"
        prefix: Qt.core.libPath + '/QtGui' + Qt.core.libInfix + '.framework/Versions/' + Qt.core.versionMajor + '/Resources/qt_menu.nib/'
        files: '*.nib'
    }

    Rule {
        inputs: [ "infoPlist" ]
        Artifact {
            fileTags: [ "installed_content" ]
            fileName: "bin/qutim.app/Contents/" + input.fileName
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.version = product.qutim_version;
            cmd.sourceCode = function() {
                var file = new TextFile(input.fileName);
                var content = file.readAll().replace(/VERSION/g, version);
                file.close();
                file = new TextFile(output.fileName, TextFile.WriteOnly);
                file.truncate();
                file.write(content);
                file.close();
            }
            cmd.description = "installing " + FileInfo.fileName(output.fileName);
            cmd.highlight = "linker";
            return cmd;
        }
    }
}
