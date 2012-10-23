Product {
    type: ["installed_content"]

    property string qutim_version_major: project.qutim_version_major
    property string qutim_version_minor: project.qutim_version_minor
    property string qutim_version_release: project.qutim_version_release
    property string qutim_version_patch: project.qutim_version_patch
    property string qutim_version: project.qutim_version

    name: "qutim-headers"

    Transformer {
        inputs: [ "libqutim/version.h.cmake" ]
        Artifact {
            fileName: "include/qutim/libqutim_version.h"
            fileTags: [ "hpp" ]
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "generating libqutim_version.h";
            cmd.highlight = "codegen";
            cmd.qutim_version = product.qutim_version;
            cmd.qutim_version_major = product.qutim_version_major;
            cmd.qutim_version_minor = product.qutim_version_minor;
            cmd.qutim_version_release = product.qutim_version_release;
            cmd.qutim_version_patch = product.qutim_version_patch;
            cmd.onWindows = (product.modules.qbs.targetOS === "windows");
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

    files: "libqutim/version.h.cmake"

    Group {
        qbs.installDir: "include/qutim"
        fileTags: ["install"]
        recursive: true
        files: "libqutim/*[a-zA-Z][a-zA-Z].h"
    }
}
