import "../Protocol.qbs" as Protocol

Protocol {
    pluginTags: 'linux'
    //sourcePath: 'src'

    cpp.defines: base.concat([
        'PURPLE_LIBDIR="/usr/lib/"' //I want to use purple.libraryPaths
    ])

    /*Depends {
        required: false
        name: "purple"
    }
    Depends {
        required: false
        name: "glib"
    }

    condition: purple.present && glib.present*/

    // TODO: yep, Probes in Modules do not work for now
    // so sorry for this hack
    condition: false

    cpp.includePaths: ["/usr/include/glib-2.0", "/usr/lib/x86_64-linux-gnu/glib-2.0/include", "/usr/include/libpurple"]
    cpp.dynamicLibraries: ["glib-2.0", "purple"]
}
