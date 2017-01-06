import qbs.base 1.0

DynamicLibrary {
    destinationDirectory: project.qutim_libexec_path

    Group {
        fileTagsFilter: product.type.concat('dynamiclibrary_symlink')
        qbs.install: true
        qbs.installDir: project.qutim_libexec_path
    }
}
