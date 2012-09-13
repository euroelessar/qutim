import "../UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    property bool useKdePhonon: qbs.targetOS === "linux"

    Depends { name: "kde.phonon"; condition: useKdePhonon }
    Depends { name: "qt.phonon"; condition: !useKdePhonon }
}
