import qbs 1.0
import qbs.FileInfo
import qbs.File
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

    type: [ 'dynamiclibrary', 'installed_content' ]
    //name: sourceDirectory
    destinationDirectory: qutim_plugin_path
    cpp.defines: [ "QUTIM_PLUGIN_ID=" + pluginId, "QUTIM_PLUGIN_NAME=\"" + name + "\""]
    cpp.visibility: 'hidden'
    cpp.installNamePrefix: "@rpath/plugins/"
    cpp.rpaths: qbs.targetOS.contains("osx")
                ? ["@loader_path/../..", "@executable_path/.."]
                : ["$ORIGIN", "$ORIGIN/..", "$ORIGIN/../.."]
    cpp.createSymlinks: false

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: [ "core", "gui", "network", "script", "widgets", "quick" ] }
    Depends { name: "libqutim" }

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: qutim_plugin_path
    }
    Group {
        name: "Source"
        prefix: (sourcePath !== '' ? sourcePath + '/' : '') + '**/'
        files: [ '*.cpp', '*.h', '*.ui', "*.c" ]
    }
    Group {
        name: "ObjectiveC sources [osx]"
        condition: qbs.targetOS.concat("osx")
        prefix: (sourcePath !== '' ? sourcePath + '/' : '') + '**/'
        files: [ '*.mm' ]
    }
    Group {
        name: "Meta information"
        fileTags: [ "pluginTemplate" ]
        files: '*.plugin.json'
    }
    Group {
        name: "Generic cpp template"
        fileTags: [ "pluginCppTemplate" ]
        files: [ templateFilePath ]
    }
    Group {
        fileTags: "dummy"
        name: "QML files [dummy]"
        files: "qml/**"
    }

    Rule {
        inputs: ["pluginTemplate"]
        multiplex: true
        explicitlyDependsOn: [ "pluginCppTemplate" ]

        Artifact {
            fileTags: [ "cpp", "moc_cpp" ]
            filePath: 'GeneratedFiles/' + product.name + '/' + product.name + "genplugin.cpp"
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.productName = product.name;
            cmd.templateFilePath = product.templateFilePath;
            cmd.inputFilePath = FileInfo.path(input.filePath);
            cmd.sourceCode = function() {
                var file = new TextFile(input.filePath, TextFile.ReadOnly);
                var data = JSON.parse(file.readAll());
                file = new TextFile(templateFilePath, TextFile.ReadOnly);
                var template = file.readAll();

                var includeQmlTypes = "";
                var registerQmlTypes = "";

                if (data.qmlTypes) {
                    for (var header in data.qmlTypes) {
                        var source = data.qmlTypes[header];

                        includeQmlTypes += '#include "' + inputFilePath + "/" + header + '"\n'
                        registerQmlTypes += source + '\n';
                    }
                }

                var variables = [
                    { from: "productName", to: productName },
                    { from: "pluginIcon", to: data.pluginIcon },
                    { from: "pluginName", to: data.pluginName },
                    { from: "pluginDescription", to: data.pluginDescription },
                    { from: "extensionHeader", to: inputFilePath + "/" + data.extensionHeader },
                    { from: "extensionClass", to: data.extensionClass },
                    { from: "includeQmlTypes", to: includeQmlTypes },
                    { from: "registerQmlTypes", to: registerQmlTypes },
                ];
                for (var i = 0; i < variables.length; ++i) {
                    var from = variables[i].from;
                    var to = variables[i].to || "";
                    template = template.replace(new RegExp("\\${" + from + "}", "g"), to);
                }
                file = new TextFile(output.filePath, TextFile.WriteOnly);
                file.truncate();
                file.write(template);
                file.close();
            }
            cmd.description = "generating " + FileInfo.fileName(output.filePath);
            cmd.highlight = "codegen";
            return cmd;
        }
    }
}
