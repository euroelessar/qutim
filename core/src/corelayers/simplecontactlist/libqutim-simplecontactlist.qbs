import "../../../../core/Framework.qbs" as Framework
import qbs.FileInfo

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

    Rule {
        inputs: [ "devheader" ]
        Artifact {
            fileTags: [ "hpp" ]
            fileName: "GeneratedFiles/libqutim/include/qutim/simplecontactlist/" + input.fileName
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
