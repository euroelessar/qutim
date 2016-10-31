import "../Protocol.qbs" as Protocol
import qbs.Probes

Protocol {
    property bool jingle: false

    Probes.PkgConfigProbe {
        id: pkgProbe
        name: "libjreen-qt5"
    }

    condition: {
        if(!pkgProbe.found) {
            if(project.systemJreen)
                print("System jreen-qt5 hasn't been found")

            return true
        }

        return pkgProbe.found
    }

    cpp.dynamicLibraries: {
        if(!project.systemJreen)
            return undefined

        var flags = [];
        var libs = (typeof pkgProbe.libs !== "undefined") ? pkgProbe.libs : "-ljreen-qt5";

        for (i in libs) {
            splitflag = libs[i].split('-l');
            if (splitflag.length == 2 && splitflag[0] == "") {
                flags.push(splitflag[1]);
            }
        }

        return flags;
    }

    cpp.cFlags: {
        if(!project.systemJreen || (typeof pkgProbe.cflags === "undefined"))
            return undefined

        return pkgProbe.cflags
    }

    cpp.cxxFlags: {
        if(!project.systemJreen || (typeof pkgProbe.cflags === "undefined"))
            return undefined

        return pkgProbe.cflags
    }

    Depends {
        name: "jreen"
        condition: !project.systemJreen
    }

    cpp.includePaths: ["src"]
    cpp.defines: {
        var basicDefs = base
        if (jingle)
            basicDefs = basicDefs.concat("JABBER_HAVE_MULTIMEDIA")
        return basicDefs
    }

    Depends {
        condition: jingle
        name: "Qt.multimedia"
    }
}
