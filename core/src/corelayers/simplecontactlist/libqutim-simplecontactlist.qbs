import "../../../../core/Framework.qbs" as Framework
import qbs.FileInfo
import qbs.TextFile

Framework {
    name: "qutim-simplecontactlist"

    Depends { name: "cpp" }
    Depends { name: "libqutim" }
    Depends { name: "Qt"; submodules: [ "core", "gui", "widgets" ] }
    Depends { name: "simplecontactlist-headers" }

    cpp.defines: "SIMPLECONTACTLIST_LIBRARY"
    cpp.visibility: 'hidden'

    Group {
        files: [
            "lib/*_p.h",
            "lib/*.cpp",
            "lib/*.ui"
        ]
    }
    Group {
        files: "lib/*.h"
        excludeFiles: "lib/*_p.h"
        fileTags: ["hpp", "devheader"]
        overrideTags: false
    }

    Export {
        Depends { name: "cpp" }

        cpp.includePaths: [
            product.buildDirectory + "/GeneratedFiles/libqutim/include"
        ]
    }

    Rule {
        inputs: [ "devheader" ]
        Artifact {
            fileTags: [ "hpp" ]
            filePath: "GeneratedFiles/libqutim/include/qutim/simplecontactlist/" + input.fileName
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.sourceCode = function() {
                var inputFile = new TextFile(input.filePath, TextFile.ReadOnly);
                var file = new TextFile(output.filePath, TextFile.WriteOnly);
                file.truncate();
                file.write("#include \"" + input.filePath + "\"\n"); //inputFile.readAll());
                file.close();
            }
            cmd.description = "generating " + FileInfo.fileName(output.filePath);
            cmd.highlight = "filegen";
            return cmd;
        }
    }
}
