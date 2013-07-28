import qbs.base 1.0

StaticLibrary {
    property string sourcePath

    destination: "lib"

    Depends { name: "cpp" }

    Group {
        name: "Source"
        condition: sourcePath !== undefined
        prefix: sourcePath + '/**/'
        files: [ '*.cpp', '*.h', '*.ui', "*.c", "*.mm" ]
    }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [
            "."
        ]
    }
}

