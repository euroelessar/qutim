import qbs.base
import qbs.FileInfo
import qbs.TextFile
import "Framework.qbs" as Framework

Framework {
    name: "libqutim"

    property string versionMajor: project.qutim_version_major
    property string versionMinor: project.qutim_version_minor
    property string versionRelease: project.qutim_version_release
    property string versionPatch: project.qutim_version_patch
    property string version: project.qutim_version
    property string shareDir: {
        if (qbs.targetOS === "mac")
            return "/Resources/share";
        else
            return ""; //qutimscope.shareDir;
    }

    //Depends { name: "qutimscope" }
    Depends { name: "k8json" }
    Depends { name: "qxt" }
    //Depends { name: "Qtdwm" }
    Depends { name: "Qtsolutions" }
    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: [ 'core', 'gui', 'network', 'script', 'declarative', 'widgets' ] }
    Depends { name: "carbon"; condition: qbs.targetOS === 'mac' }
    Depends { name: "cocoa"; condition: qbs.targetOS === 'mac' }
    //Depends { name: "windows.user32"; condition: qbs.targetOS === 'windows' }
    //Depends { name: "windows.gdi32"; condition: qbs.targetOS === 'windows' } //in product module it's doesn't work
    //Depends { name: "x11"; condition: qbs.targetOS === 'linux' }

    cpp.includePaths: [
        "libqutim",
        product.buildDirectory + "/GeneratedFiles/libqutim/include/qutim"
    ]

    cpp.dynamicLibraryPrefix: ""
    cpp.staticLibraryPrefix: ""
    cpp.defines: [
        "LIBQUTIM_LIBRARY",
        "QUTIM_SHARE_DIR=\"" + shareDir + "\"",
        "QUTIM_SINGLE_PROFILE"
    ]
    cpp.cxxFlags: [
        "-std=c++11"
    ]

//    Properties {
//        condition: project.singleProfile
//        cpp.defines: base.concat([
//            "QUTIM_SINGLE_PROFILE"
//        ])
//    }

    Export {
        property string basePath

        Depends { name: "cpp" }
        cpp.includePaths: [
            product.buildDirectory + "/GeneratedFiles/libqutim/include",
            product.buildDirectory + "/GeneratedFiles/libqutim/include/qutim",
            "3rdparty/slidingstackedwidget",
            "3rdparty/flowlayout",
            "3rdparty/"
        ]
        Properties {
            condition: project.declarativeUi
            cpp.defines: "QUTIM_DECLARATIVE_UI"
        }
        Properties {
            condition: project.singleProfile
            cpp.defines: "QUTIM_SINGLE_PROFILE"
        }
    }

    files: [
        'libqutim/**/*.cpp',
        'libqutim/**/*_p.h',
        'libqutim/version.h.cmake'
    ]

    Group {
        name: "Dev headers"
        files: 'libqutim/**/*.h'
        excludeFiles: 'libqutim/**/*_p.h'
        fileTags: ["hpp", "devheader"]
    }

    //TODO separate this libraries like qutim-adiumwebview
    Group {
        name: "SlidingStackedWidget"
        prefix: "3rdparty/slidingstackedwidget/"
        files: ["*.h", "*.cpp"]
    }
    Group {
        name: "FlowLayout"
        prefix: "3rdparty/flowlayout/"
        files: ["*.h", "*.cpp"]
    }

    Transformer {
        inputs: [ "libqutim/version.h.cmake" ]
        Artifact {
            fileName: "GeneratedFiles/libqutim/include/qutim/libqutim_version.h"
            fileTags: [ "hpp" ]
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "generating libqutim_version.h";
            cmd.highlight = "filegen";
            cmd.qutim_version = product.version;
            cmd.qutim_version_major = product.versionMajor;
            cmd.qutim_version_minor = product.versionMinor;
            cmd.qutim_version_release = product.versionRelease;
            cmd.qutim_version_patch = product.versionPatch;
            cmd.onWindows = (product.moduleProperty("qbs", "targetOS") === "windows");
            cmd.sourceCode = function() {
                var file = new TextFile(input.fileName);
                var content = file.readAll();
                // replace Windows line endings
                if (onWindows)
                    content = content.replace(/\r\n/g, "\n");
                content = content.replace(/\${CMAKE_QUTIM_VERSION_STRING}/g, qutim_version);
                content = content.replace(/\${CMAKE_QUTIM_VERSION_MAJOR}/g, qutim_version_major);
                content = content.replace(/\${CMAKE_QUTIM_VERSION_MINOR}/g, qutim_version_minor);
                content = content.replace(/\${CMAKE_QUTIM_VERSION_SECMINOR}/g, qutim_version_release);
                content = content.replace(/\${CMAKE_QUTIM_VERSION_PATCH}/g, qutim_version_patch);
                file = new TextFile(output.fileName, TextFile.WriteOnly);
                file.truncate();
                file.write(content);
                file.close();
            }
            return cmd;
        }
    }

    Rule {
        inputs: [ "devheader" ]
        Artifact {
            fileTags: [ "hpp" ]
            fileName: "GeneratedFiles/libqutim/include/qutim/" + input.fileName
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
