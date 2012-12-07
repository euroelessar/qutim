import qbs.base 1.0
import qbs.fileinfo as FileInfo

Product {
    type: "installed_content"
    name: "nameless-artwork"

    property bool installConfig: true
    property bool installSoundTheme: true
    property bool installIcons: true
    property bool installOxygenTheme: qbs.targetOS !== "linux"
    property bool installUbuntuTheme: qbs.targetOS === "linux"
    property string qutim_version: project.qutim_version

    Depends { name: "qt.core" }
    Depends { name: "artwork" }
    
    property string shareDir: artwork.shareDir

    Group {  // config files
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

    Group { // webkitstyle and ca-certs
        fileTags: [ "artwork" ]
        artwork.basePath: "share/qutim"
        prefix: "share/qutim/"
        recursive: true
        files: "*"
    }

    Group { //sounds
        condition: installSoundTheme
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork"
        prefix: "../artwork/sounds/"
        recursive: true
        files: "*"
    }

    Group { // desktop file
        condition: qbs.targetOS === "linux"
        fileTags: [ "install" ]
        prefix: "share/applications/"
        recursive: true
        files: "*"
    }

    Group { // qutim.png and default tray icons
        condition: installIcons && qbs.targetOS === "linux"
        fileTags: [ "artwork" ]
        artwork.shareDir: "share"
        artwork.basePath: "../artwork/icons/qutim"
        prefix: "../artwork/icons/qutim/"
        recursive: true
        files: "*"
    }
    
    Group { // default icon theme
        condition: installIcons
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork"
        prefix: "../artwork/icons/qutim-default/"
        recursive: true
        files: "*"
    }

    Group { // user-status icons
        condition: installIcons
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork/icons/user-status"
        qbs.installDir: "icons"
        prefix: "../artwork/icons/user-status/"
        recursive: true
        files: "*"
    }

    Group { // humanity icons
        condition: installIcons && qbs.targetOS === "linux"
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork/icons/humanity"
        qbs.installDir: "icons"
        prefix: "../artwork/icons/humanity/"
        recursive: true
        files: "*"
    }

    Group { // Mac tray icons
        condition: installIcons && qbs.targetOS === "mac"
        fileTags: [ "install" ]
        qbs.installDir: shareDir + "/icons/qutim-default/scalable/status/"
        files: "../artwork/icons/tray/MacOS/*.svg"
    }

    Group {
        condition: installIcons && installOxygenTheme
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork"
        prefix: "../artwork/icons/oxygen/"
        recursive: true
        files: "*"
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
        condition: qbs.targetOS === "mac"
        fileTags: [ "install" ]
        qbs.installDir: "qutim.app/Contents/Resources"
        files: ["qutim.icns", "qt.conf"]
    }

    Group {
        condition: qbs.targetOS === "mac"
        fileTags: [ "infoPlist" ]
        files: "Info.plist"
    }

    Group {
        condition: qbs.targetOS === "mac"
        fileTags: [ "install" ]
        qbs.installDir: "qutim.app/Contents/Resources/qt_menu.nib/"
        prefix: qt.core.libPath + '/QtGui' + qt.core.libInfix + '.framework/Versions/' + qt.core.versionMajor + '/Resources/qt_menu.nib/'
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
