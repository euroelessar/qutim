import qbs.base 1.0

DynamicLibrary {
    destinationDirectory: {
        if (qbs.targetOS === 'windows')
            return "bin";
        else
            return "lib";
    }
}
