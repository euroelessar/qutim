import qbs.base 1.0
import qbs.fileinfo as FileInfo

Product {
    type: "installed_content"
    Depends { name: "qutimscope" }

    property string shareDir: qutimscope.shareDir
    property string lconvertPath: qt.core.binPath + "/lconvert"
    property string lreleasePath: qt.core.binPath + "/lrelease"

    Depends { name: "qt.core" }

    Group {
        fileTags: [ "po" ]
        prefix: "translations/modules/"
        files: "*.po"
        recursive: true
    }

    Rule {
        inputs: [ "po" ]
        Artifact {
            fileTags: [ "qutim_ts" ]
            fileName: "GeneratedFiles/" + input.baseDir + "/" + input.fileName.replace(/.po$/, ".ts")
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.executable = product.lconvertPath;
            cmd.sourceCode = function() {
                var p = new Process();
                if (p.exec(executable, [ input.fileName, '-of', "ts" ]) !== 0)
                    throw "Cannot execute " + executable;
                var content = p.readAll().replace(/<message>/g, '<message utf8="true">');
                var file = new TextFile(output.fileName, TextFile.WriteOnly);
                file.write(content);
                file.close();
            }
            var moduleName = FileInfo.fileName(FileInfo.path(output.fileName));
            cmd.description = 'converting to ' + moduleName + "/" + FileInfo.fileName(output.fileName);
            cmd.highlight = 'codegen';
            return cmd;
        }
    }

    Rule {
        inputs: [ "qutim_ts" ]
        Artifact {
            fileTags: [ "installed_content" ]
            fileName: {
                var language = input.baseName;
                var module = FileInfo.fileName(input.baseDir);
                return product.shareDir + "/languages/" + language + "/" + module + ".qm";
            }
        }

        prepare: {
            var cmd = new Command(product.lreleasePath, [ '-silent', input.fileName, '-qm', output.fileName ]);
            var moduleName = FileInfo.fileName(FileInfo.path(output.fileName));
            cmd.description = 'converting to ' + moduleName + "/" + FileInfo.fileName(output.fileName);
            cmd.highlight = 'linker';
            return cmd;
        }
    }
}
