import qbs.base 1.0
import qbs.fileinfo as FileInfo

Product {
    type: "installed_content"
    qbs.installDir:  {
        if (qbs.targetOS === "mac")
            return "qutim.app/Contents/Resources/share"
        else
            return project.shareDir
    }

    property bool installSoundTheme: true
    property bool installIcons: true
    property bool installOxygenTheme: true
    property bool installUbuntuTheme: true

    Depends { name: "qutimscope" }

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
        qbs.installDir: project.shareDir + "/icons/qutim-default/scalable/status/"
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
        qbs.installDir: project.shareDir + "/icons/ubuntu-mono-light/scalable/status/"
        files: "../artwork/icons/tray/ubuntu-mono-light/*.svg"
    }

    Group {
        condition: installIcons && installUbuntuTheme
        fileTags: [ "install" ]
        qbs.installDir: project.shareDir + "/icons/ubuntu-mono-dark/scalable/status/"
        files: "../artwork/icons/tray/ubuntu-mono-dark/*.svg"
    }

    Rule {
        inputs: [ "artwork" ]
        Artifact {
            fileTags: [ "installed_content" ]
            fileName: {
                var basePath = input.modules.qutimscope.basePath;
                var fileName = input.baseDir + "/" + input.fileName;
                var relativePath = FileInfo.relativePath(basePath, fileName);
                return input.modules.qbs.installDir + "/" + relativePath;
            }
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.sourceCode = function() {
                File.remove(output.fileName);
                if (!File.copy(input.fileName, output.fileName))
                    throw "Cannot install '" + input.fileName + "' as '" + output.fileName + "'";
            }
            cmd.description = "installing " + FileInfo.fileName(output.fileName);
            cmd.highlight = "linker";
            return cmd;
        }

    }
}
