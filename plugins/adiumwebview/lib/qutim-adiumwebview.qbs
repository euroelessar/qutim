import "../../../core/Framework.qbs" as Framework

Framework {
    name: "qutim-adiumwebview"
   
    property string versionMajor: '0'
    property string versionMinor: '1'
    property string versionRelease: '0'
    property string version: versionMajor+'.'+versionMinor+'.'+versionRelease
   
    moduleSearchPaths: "../../../qbs/modules"

    type: ["dynamiclibrary", "installed_content"]
 
    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.gui" }
    Depends { name: "Qt.network" }
    Depends { name: "Qt.webkit" }
    Depends { name: "libqutim" }
 
    //cpp.warningLevel: "all"
    cpp.includePaths: [
        "."
    ]
    cpp.defines: [
        "ADIUMWEBVIEW_LIBRARY"
    ]
    
    files: [
        "*.h",
        "*.cpp"
    ]

}
