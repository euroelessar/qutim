import qbs.base 1.0
import qbs.fileinfo as FileInfo

Module {
    property string basePath
    property string shareDir: qutimscope.shareDir

    Depends { name: "qutimscope" }

    Rule {
        inputs: [ "artwork" ]
        Artifact {
            fileTags: [ "installed_content" ]
            fileName: {
                var basePath = input.modules.artwork.basePath;
                var fileName = input.baseDir + "/" + input.fileName;
                var relativePath = FileInfo.relativePath(basePath, fileName);
                return input.modules.artwork.shareDir + "/" + input.modules.qbs.installDir + "/" + relativePath;
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
