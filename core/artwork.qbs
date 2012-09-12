import qbs.base 1.0
import qbs.fileinfo as FileInfo

Product {
    type: "installed_content"
    name: "nameless-artwork"

    property bool installConfig: true
    property bool installSoundTheme: true
    property bool installIcons: true
    property bool installOxygenTheme: true
    property bool installUbuntuTheme: true
    property string qutim_version: project.qutim_version

    Depends { name: "qt.core" }
    Depends { name: "artwork" }
    
    property string shareDir: artwork.shareDir

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
        artwork.basePath: "share"
        prefix: "share/qutim/"
        recursive: true
        files: "*"
    }

    Group {
        condition: installSoundTheme
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork"
        prefix: "../artwork/sounds/"
        recursive: true
        files: "*"
    }

    Group {
        condition: installIcons
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork"
        prefix: "../artwork/icons/qutim-default/"
        recursive: true
        files: "*"
    }

    Group {
        condition: installIcons
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork"
        prefix: "../artwork/icons/user-status/hicolor/"
        recursive: true
        files: "*"
    }

    Group {
        condition: installIcons && qbs.targetOS === "linux"
        fileTags: [ "artwork" ]
        artwork.basePath: "../artwork"
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
        artwork.basePath: "../artwork"
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

    Group {
        condition: qbs.targetOS === "mac"
        fileTags: [ "install" ]
        qbs.installDir: "qutim.app/Contents/Resources"
        files: "qutim.icns"
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
