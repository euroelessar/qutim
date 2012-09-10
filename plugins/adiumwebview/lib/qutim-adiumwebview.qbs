Product {
    name: "qutim-adiumwebview"
   
    property string versionMajor: '0'
    property string versionMinor: '1'
    property string versionRelease: '0'
    property string version: versionMajor+'.'+versionMinor+'.'+versionRelease
   
    moduleSearchPaths: "../../../qbs/modules"
 
    destination: "lib"
    type: ["dynamiclibrary", "installed_content"]
 
    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.gui" }
    Depends { name: "Qt.webkit" }
 
    //cpp.warningLevel: "all"
    cpp.includePaths: [
        "."
    ]
    
    files: [
        "*.h",
        "*.cpp"
    ]

}
