import qbs.base 1.0
import qbs.fileinfo as FileInfo

Module {
    Depends { name: "Qt.core" }

    property string pluginPath: FileInfo.joinPaths(qt.core.libPath, "/../plugins/")
    property string application: product.name + ".app"
    property string pluginDestination: application + "/Contents/PlugIns/"

    function qtPluginName(base) {
        var prefix = "lib";
        var suffix = ".dylib";
        return prefix + base + suffix;
    }

    Rule {
        inputs: [ "plugins" ]
        Artifact {
            fileTags: [ "installed_content" ]
            fileName: {
                var basePath = input.moduleProperty('deploy', 'pluginPath');
                var fileName = basePath + input.fileName;
                return input.moduleProperty('deploy', 'pluginDestination') + "/" + input.fileName;
            }
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.sourceCode = function() {
                File.remove(output.fileName);
                if (!File.copy(input.fileName, output.fileName))
                    throw "Cannot install '" + input.fileName + "' as '" + output.fileName + "'";
                //TODO don't copy unnecessarily files
            }
            cmd.description = "deploying qt "+ FileInfo.fileName(output.fileName) + " plugins";
            cmd.highlight = "linker";
            return cmd;
        }

    }
}
