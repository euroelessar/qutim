import "../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    type: ['installed_content', 'dynamiclibrary']

    Depends { name: "vreen" }
    Depends { name: "vreenoauth" }
    Depends { name: "qt.declarative" }
    Depends { name: "qt.webkit" }
    Depends { name: "artwork" }

    cpp.includePaths: ["src"]
    cpp.cxxFlags: base

    Group {
        fileTags: [ "artwork" ]
        artwork.basePath: "share"
        prefix: "share/vphotoalbum/"
        files: '**'
    }

    Properties {
        condition: qbs.toolchain !== 'msvc'
        cpp.cxxFlags: outer.concat([ "-std=c++11" ])
    }
    Properties {
        condition: qbs.targetOS === "mac"
        cpp.cxxFlags: outer.concat([ "-stdlib=libc++" ])
    }
}
