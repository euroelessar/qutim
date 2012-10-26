import qbs.base 1.0

DynamicLibrary {
    destination: {
        if (qbs.targetOS === "mac")
            return "qutim.app/Contents/MacOS";
        else if (qbs.targetOS === 'windows')
            return "bin";
        else
            return "lib";
    }
}
