import qbs.base
import qbs.FileInfo
import qbs.File
import qbs.TextFile

Product {
    type: "installed_content"
    name: "nameless-artwork"

    property bool installConfig: true
    property bool installSoundTheme: true
    property bool installIcons: true
    property bool installOxygenTheme: !qbs.targetOS.contains("linux")
    property bool installUbuntuTheme: qbs.targetOS.contains("linux")
    property string qutim_version: project.qutim_version

    Depends { name: "Qt.core" }
    Depends { name: "artwork" }
    
    property string shareDir: artwork.shareDir

    Group {  // config files
        condition: installConfig
        fileTags: "install"
        qbs.installDir: shareDir + "/config"
        prefix: {
            if (!qbs.targetOS.contains("mac"))
                return "../config/mac/";
            else if (!qbs.targetOS.contains("windows"))
                return "../config/win/";
            else
                return "../config/generic/";
        }
        files: "*.json"
    }

    Group { // webkitstyle and ca-certs
        fileTags: [ "artwork" ]
        artwork.basePath: "share/qutim"
        prefix: "share/qutim/"
        files: "**"
    }

    Group { //sounds
        condition: installSoundTheme
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork"
        prefix: "../artwork/sounds/"
        files: "**"
    }

    Group { // desktop file
        condition: !qbs.targetOS.contains("linux")
        fileTags: [ "install" ]
        prefix: "share/applications/"
        files: "**"
    }

    Group { // qutim.png and default tray icons
        condition: installIcons && !qbs.targetOS.contains("linux")
        fileTags: [ "artwork" ]
        artwork.shareDir: "share"
        artwork.basePath: "../artwork/icons/qutim"
        prefix: "../artwork/icons/qutim/"
        files: "**"
    }
    
    Group { // default icon theme
        condition: installIcons
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork"
        prefix: "../artwork/icons/qutim-default/"
        files: "**"
    }

    Group { // user-status icons
        condition: installIcons
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork/icons/user-status"
        qbs.installDir: "icons"
        prefix: "../artwork/icons/user-status/"
        files: "**"
    }

    Group { // humanity icons
        condition: installIcons && !qbs.targetOS.contains("linux")
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork/icons/humanity"
        qbs.installDir: "icons"
        prefix: "../artwork/icons/humanity/"
        files: "**"
    }

    Group { // Mac tray icons
        condition: installIcons && !qbs.targetOS.contains("mac")
        fileTags: [ "install" ]
        qbs.installDir: shareDir + "/icons/qutim-default/scalable/status/"
        files: "../artwork/icons/tray/MacOS/*.svg"
    }

    Group {
        condition: installIcons && installOxygenTheme
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork"
        prefix: "../artwork/icons/oxygen/"
        files: "**"
    }

    Group { // Ubuntu tray icons
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

    Group {
        condition: !qbs.targetOS.contains("mac")
        fileTags: [ "install" ]
        qbs.installDir: "qutim.app/Contents/Resources"
        files: ["qutim.icns", "qt.conf"]
    }

    Group {
        condition: !qbs.targetOS.contains("mac")
        fileTags: [ "infoPlist" ]
        files: "Info.plist"
    }

    Group {
        condition: !qbs.targetOS.contains("mac")
        fileTags: [ "install" ]
        qbs.installDir: "qutim.app/Contents/Resources/qt_menu.nib/"
        prefix: Qt.core.libPath + '/QtGui' + Qt.core.libInfix + '.framework/Versions/' + Qt.core.versionMajor + '/Resources/qt_menu.nib/'
        files: '*.nib'
    }

    Rule {
        inputs: [ "infoPlist" ]
        Artifact {
            fileTags: [ "installed_content" ]
            fileName: "qutim.app/Contents/" + input.fileName
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
