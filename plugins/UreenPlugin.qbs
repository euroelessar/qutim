import qbs 1.0
import qbs.FileInfo
import qbs.TextFile

Product {
    condition: {
        var tags = project.pluginTags;
        var selectedTags = [].concat(pluginTags);
        for (var i in selectedTags) {
            if (tags.indexOf(selectedTags[i]) !== -1)
                return true;
        }
        print('Skipping plugin ' + name + '(selected tags: ' + tags + ' | plugin tags: ' + pluginTags + ')')
        return false;
    }

    property string templateFilePath: path + "/plugintemplate.cpp"
    property string projectPath: project.path
    property string sourcePath: "src"
    property var pluginTags: ['core']
    // FIXME: add Cache support
    property string pluginId: {
        function numberToHex(number, length) {
            if (number < 0)
                number = (~number) - 1;
            var str = '';
            str = number.toString(16);
            while (str.length < length)
                str = '0' + str;
            return str;
        }

        function hashCode(str) {
            if (str.length === 0)
                return 0;
            var hash = [0, 0];
            for (var i = 0; i < str.length; i++) {
                var ch = str.charCodeAt(i);
                hash[i & 1] = ((hash[i & 1] << 5) - hash[i & 1]) + ch;
                hash[i & 1] = hash[i & 1] & hash[i & 1];
            }
            return numberToHex(hash[0], 8) + numberToHex(hash[1], 8);
        }
        //print(name, hashCode(name));
        return hashCode(name);
    }

    type: 'dynamiclibrary'
    name: FileInfo.fileName(sourceDirectory);
    destination: {
        if (qbs.targetOS === 'mac')
            return "qutim.app/Contents/PlugIns";
        else if (qbs.targetOS === 'windows')
            return "bin/plugins"
        else
            return "lib/qutim/plugins";
    }
    cpp.defines: [ "QUTIM_PLUGIN_ID=" + pluginId ]
    cpp.rpaths: ["$ORIGIN/../../", "$ORIGIN"]
    cpp.visibility: 'hidden'

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: [ "core", "gui", "network", "script", "widgets" ] }
    Depends { name: "libqutim" }

    Group {
        prefix: (sourcePath !== '' ? sourcePath + '/' : '') + '**/'
        files: [ '*.cpp', '*.h', '*.ui', "*.c" ]
    }
    Group {
        condition: qbs.targetOS === 'mac'
        prefix: (sourcePath !== '' ? sourcePath + '/' : '') + '**/'
        files: [ '*.mm' ]
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
            cmd.templateFilePath = product.templateFilePath;
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
