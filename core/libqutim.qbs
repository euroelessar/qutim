import qbs.base
import qbs.FileInfo
import qbs.TextFile
import "Framework.qbs" as Framework

Framework {
    name: "libqutim"

    property string versionMajor: qutim_version_major
    property string versionMinor: qutim_version_minor
    property string versionRelease: qutim_version_release
    property string versionPatch: qutim_version_patch
    property string version: qutim_version
    property string shareDir: qutim_share_path

    Depends { name: "k8json" }
    Depends { name: "qxt" }
    Depends { name: "Qtsolutions" }
    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: [ 'core', 'gui', 'network', 'script', 'quick', 'widgets' ] }

    cpp.includePaths: [
        "libqutim",
        product.buildDirectory + "/GeneratedFiles/libqutim/include/qutim"
    ]

    Properties {
        condition: qbs.targetOS.contains("linux")
        cpp.dynamicLibraries: ["Qt5X11Extras", "X11"];
    }

    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.dynamicLibraries: [ "user32" ]
    }

    cpp.dynamicLibraryPrefix: ""
    cpp.staticLibraryPrefix: ""
    cpp.defines: {
        var sharePath = qbs.targetOS.contains("osx") ? "Resources/share"
                                                     : qutim_share_path;
        var defines = [
                    "LIBQUTIM_LIBRARY",
                    "QUTIM_SHARE_DIR=\"" + sharePath + "\"",
                    "QUTIM_SINGLE_PROFILE",
                    "QUTIM_PLUGIN_NAME=\"libqutim\""
                ];
        return defines;
    }
    cpp.cxxFlags: {
        var flags = base.concat("-std=c++11");
        if (qbs.toolchain.contains("clang"))
            flags = flags.concat("-stdlib=libc++");
        return flags;
    }

    Properties {
        condition: qbs.targetOS.contains("osx")
        cpp.frameworks: ["Cocoa", "Carbon" ]
    }

    Export {
        Depends { name: "cpp" }

        cpp.includePaths: [
            product.buildDirectory + "/GeneratedFiles/libqutim/include",
            product.buildDirectory + "/GeneratedFiles/libqutim/include/qutim",
            "3rdparty/slidingstackedwidget",
            "3rdparty/flowlayout",
            "3rdparty/"
        ]
        cpp.cxxFlags: {
            var flags = base.concat("-std=c++11");
            if (qbs.toolchain.contains("clang"))
                flags = flags.concat("-stdlib=libc++");
            return flags;
        }
        cpp.linkerFlags: {
            var flags = base;
            if (qbs.toolchain.contains("clang") && qbs.targetOS.contains("linux"))
                flags = flags.concat("-stdlib=libc++ -lcxxrt");
            return flags;
        }

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
            filePath: "GeneratedFiles/libqutim/include/qutim/libqutim_version.h"
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
                var file = new TextFile(input.filePath);
                var content = file.readAll();
                // replace Windows line endings
                if (onWindows)
                    content = content.replace(/\r\n/g, "\n");
                content = content.replace(/\${CMAKE_QUTIM_VERSION_STRING}/g, qutim_version);
                content = content.replace(/\${CMAKE_QUTIM_VERSION_MAJOR}/g, qutim_version_major);
                content = content.replace(/\${CMAKE_QUTIM_VERSION_MINOR}/g, qutim_version_minor);
                content = content.replace(/\${CMAKE_QUTIM_VERSION_SECMINOR}/g, qutim_version_release);
                content = content.replace(/\${CMAKE_QUTIM_VERSION_PATCH}/g, qutim_version_patch);
                file = new TextFile(output.filePath, TextFile.WriteOnly);
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
            filePath: "GeneratedFiles/libqutim/include/qutim/" + input.fileName
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.sourceCode = function() {
                var inputFile = new TextFile(input.filePath, TextFile.ReadOnly);
                var file = new TextFile(output.filePath, TextFile.WriteOnly);
                file.truncate();
                file.write("#include \"" + input.filePath + "\"\n");
                file.close();
            }
            cmd.description = "generating " + FileInfo.fileName(output.filePath);
            cmd.highlight = "filegen";
            return cmd;
        }
    }

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: qutim_libexec_path
    }
}
