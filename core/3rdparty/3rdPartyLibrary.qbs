import qbs.base 1.0

StaticLibrary {
    destination: "lib"
    Depends { name: "cpp" }
    Depends { name: "qutim-headers" }

    ProductModule {
        Depends { name: "cpp" }
        cpp.includePaths: [
            "."
        ]
    }
}

