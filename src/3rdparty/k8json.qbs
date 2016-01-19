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

    files: [
        "k8json/k8json.h",
        "k8json/k8json.cpp"
    ]
}

