import qbs.base 1.0

StaticLibrary {
    property string sourcePath

    destinationDirectory: "lib"

    Depends { name: "cpp" }

    Group {
        name: "Source"
        condition: sourcePath !== undefined
        prefix: sourcePath + '/**/'
        files: [ '*.cpp', '*.h', '*.ui', "*.c", "*.mm" ]
    }

    cpp.objcxxFlags: {
        var flags = base.concat("-std=c++11");
        if (qbs.toolchain.contains("clang"))
            flags = flags.concat("-stdlib=libc++");
        return flags;
    }
    cpp.cxxFlags: {
        var flags = base.concat("-std=c++11");
        if (qbs.toolchain.contains("clang"))
            flags = flags.concat("-stdlib=libc++");
        return flags;
    }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [
            "."
        ]
    }
}

