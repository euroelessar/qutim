import qbs.base 1.0

StaticLibrary {
    name: "k8json"
    destinationDirectory: "lib"

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: 'core' }

    cpp.defines: [
        "K8JSON_INCLUDE_GENERATOR",
        "K8JSON_INCLUDE_COMPLEX_GENERATOR"
    ]

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

    files: [
        "k8json/k8json.h",
        "k8json/k8json.cpp"
    ]
}

