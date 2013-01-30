import qbs.base 1.0
import qbs.fileinfo 1.0 as FileInfo
import "../../qbs/imports/probes" as Probes

Module {

    Depends { name: "cpp" }

    Probes.LibraryProbe {
        id: libraryProbe
        names: ["archive", "libarchive"]
    }

    cpp.dynamicLibraries: {
        if (libraryProbe.filePath) {
            print("LibraryProbe: found library " + libraryProbe.filePath);
            return libraryProbe.filePath;
        } else {
            print("LibraryProbe: could not found library " + libraryNames);
        }
    }
}
