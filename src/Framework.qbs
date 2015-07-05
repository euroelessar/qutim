import qbs.base 1.0

DynamicLibrary {
    destinationDirectory: qutim_libexec_path

    Group {
        fileTagsFilter: product.type.concat('dynamiclibrary_symlink')
        qbs.install: true
        qbs.installDir: qutim_libexec_path
    }
}
