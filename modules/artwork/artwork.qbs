import qbs.base 1.0
import qbs.fileinfo as FileInfo

Module {
   // property string shareDir: ""
//    qbs.installDir: { print("sharedir->>>>>>>>>>>>" + project.shareDir); return project.shareDir }
    property string shareDir: {
        if (qbs.targetOS === "mac")
            return "qutim.app/Contents/Resources/share"
        else
            return "share"
    }

    qbs.installDir: shareDir
    
    Depends { name: "qutimscope" }

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
