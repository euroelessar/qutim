import qbs.base 1.0
import qbs.fileinfo as FileInfo

Product {
    condition: {
        var tags = [ 'core' ];
        if (qbs.targetOS === 'mac')
            tags.concat('mac');
        if (qbs.targetOS === 'linux')
            tags.concat('linux', 'kde');
        if (qbs.targetOS === 'windows')
            tags.concat('windows');
        tags.concat('desktop');

        for (var i in pluginTags) {
            if (tags.indexOf(pluginTags[i]) !== -1)
                return true;
        }
        return false;
    }

    property string projectPath: project.path
    property string sourcePath: "src"
    property var pluginTags: ['core']

    type: 'dynamiclibrary'
    name: FileInfo.fileName(product.path);
    destination: {
        if (qbs.targetOS === 'mac')
            return "qutim.app/Contents/PlugIns";
        else
            return "lib/qutim/plugins";
    }
    // FIXME
    cpp.defines: [ "QUTIM_PLUGIN_ID=0123456789abcdef" ]
    cpp.rpaths: ["$ORIGIN/../../", "$ORIGIN"]
    cpp.visibility: 'hidden'

    Depends { name: "cpp" }
    Depends { name: "qt"; submodules: [ "core", "gui", "network", "script" ] }
    Depends { name: "libqutim" }
    Depends { name: "qutimscope" }

    Group {
        prefix: sourcePath !== '' ? sourcePath + '/' : ''
        files: [ '*.cpp', '*.h', '*.ui', "*.c" ]
        recursive: true
    }
    Group {
        condition: qbs.targetOS === 'mac'
        prefix: sourcePath !== '' ? sourcePath + '/' : ''
        files: [ '*.mm' ]
        recursive: true
    }
    Group {
        fileTags: [ "pluginTemplate" ]
        files: '*.plugin.json'
    }

    Rule {
        inputs: ["pluginTemplate"]
        multiplex: true

        Artifact {
            fileTags: [ "cpp", "moc_cpp" ]
            fileName: 'GeneratedFiles/' + product.name + '/' + product.name + "genplugin.cpp"
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.productName = product.name;
            cmd.templateFilePath = product.projectPath + "/plugins/plugintemplate.cpp";
            cmd.inputFilePath = FileInfo.path(input.fileName);
            cmd.sourceCode = function() {
                var file = new TextFile(input.fileName, TextFile.ReadOnly);
                var data = JSON.parse(file.readAll());
                file = new TextFile(templateFilePath, TextFile.ReadOnly);
                var template = file.readAll();
                var variables = [
                    { from: "productName", to: productName },
                    { from: "pluginIcon", to: data.pluginIcon },
                    { from: "pluginName", to: data.pluginName },
                    { from: "pluginDescription", to: data.pluginDescription },
                    { from: "extensionHeader", to: inputFilePath + "/" + data.extensionHeader },
                    { from: "extensionClass", to: data.extensionClass }
                ];
                for (var i = 0; i < variables.length; ++i) {
                    var from = variables[i].from;
                    var to = variables[i].to || "";
                    template = template.replace(new RegExp("\\${" + from + "}", "g"), to);
                }
                file = new TextFile(output.fileName, TextFile.WriteOnly);
                file.truncate();
                file.write(template);
                file.close();
            }
            cmd.description = "generating " + FileInfo.fileName(output.fileName);
            cmd.highlight = "codegen";
            return cmd;
        }
    }
}
