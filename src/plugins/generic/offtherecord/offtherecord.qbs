import "../GenericPlugin.qbs" as GenericPlugin
import qbs.Probes

GenericPlugin {

    // WARNING: Probe may be buggy in qbs 1.3.3 and 1.4.1
    Probes.PkgConfigProbe {
        id: pkgConfig
        name: "libotr"
        minVersion: "4.0.0"
    }
    
    condition: {
        if(!pkgConfig.found) {
            print("libotr5 not found, offtherecord plugin will be skipped")
            print("Nevertheless, maybe it's a bug of QBS PkgProbe, so if you know that library is presented, just pass offtherecord.condition:true")
        }

        return pkgConfig.found
    }

    pluginTags: ['linux', 'crypto', 'mac']
    
    cpp.dynamicLibraries: {

        var flags = [];
        var libs = (typeof pkgConfig.libs !== "undefined") ? pkgConfig.libs : "-lotr";

        for (i in libs) {
            splitflag = libs[i].split('-l');
            if (splitflag.length == 2 && splitflag[0] == "") {
                flags.push(splitflag[1]);
            }
        }

        return flags;
    }
}
