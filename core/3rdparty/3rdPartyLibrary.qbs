import qbs.base 1.0

StaticLibrary {
    property string sourcePath

    destination: "lib"

    Depends { name: "cpp" }
    Depends { name: "qutim-headers" }

    Group {
        condition: sourcePath !== null
        prefix: sourcePath + '/'
        files: [ '*.cpp', '*.h', '*.ui', "*.c" ]
        recursive: true
    }
    Group {
        condition: sourcePath !== null && qbs.targetOS === 'mac'
        prefix: sourcePath + '/'
        files: [ '*.mm' ]
        recursive: true
    }

    ProductModule {
        Depends { name: "cpp" }
        cpp.includePaths: [
            "."
        ]
    }
}

