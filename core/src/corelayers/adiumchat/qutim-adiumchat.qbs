import "../../../../core/Framework.qbs" as Framework
import qbs.FileInfo
import qbs.TextFile

Framework {
    name: "qutim-adiumchat"
    type: ["dynamiclibrary", "installed_content"]

    Depends { name: "cpp" }
    Depends { name: "libqutim" }
    Depends { name: "Qt"; submodules: [ "core", "gui", "widgets" ] }

    cpp.defines: [
        "ADIUMCHAT_LIBRARY",
        "QUTIM_PLUGIN_NAME=\"" + name + "\""
    ]
    cpp.visibility: 'hidden'

    files: "lib/*.cpp"

    Group {
        qbs.installDir: "include/qutim/adiumchat"
        fileTags: ["install", "devheader"]
        overrideTags: false
        files: [
            "lib/*.h"
        ]
    }

    Rule {
        inputs: [ "devheader" ]
        Artifact {
            fileTags: [ "hpp" ]
            fileName: "GeneratedFiles/libqutim/include/qutim/adiumchat/" + input.fileName
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.sourceCode = function() {
                var inputFile = new TextFile(input.fileName, TextFile.ReadOnly);
                var file = new TextFile(output.fileName, TextFile.WriteOnly);
                file.truncate();
                file.write("#include \"" + input.fileName + "\"\n"); //inputFile.readAll());
                file.close();
            }
            cmd.description = "generating " + FileInfo.fileName(output.fileName);
            cmd.highlight = "filegen";
            return cmd;
        }
    }
}
