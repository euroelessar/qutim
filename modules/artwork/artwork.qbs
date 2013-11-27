import qbs.base 1.0
import qbs.FileInfo
import qbs.File

Module {
    property string basePath
    property string shareDir: qutim_share_path

    Rule {
        inputs: [ "artwork" ]
        Artifact {
            fileTags: [ "installed_content" ]
            fileName: {
                var basePath = input.moduleProperty('artwork', 'basePath');
                var fileName = input.baseDir + "/" + input.fileName;
                var relativePath = FileInfo.relativePath(basePath, fileName);

                var path = input.moduleProperty('artwork', 'shareDir') + "/";
                if (input.moduleProperty('qbs', 'installDir')  !== undefined)
                    path += input.moduleProperty('qbs', 'installDir') + "/";
                path += relativePath;
                return path;
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
