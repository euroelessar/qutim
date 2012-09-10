import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    Depends { name: "adiumchat" }
    Depends { name: "qt.webkit" }
    Depends { name: "qutim-adiumwebview" }
}
