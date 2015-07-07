import qbs.base 1.0

Project {
    name: "Integration plugins"

    references: [
        "indicator/indicator.qbs",
        "linuxintegration/linuxintegration.qbs",
        "macintegration/macintegration.qbs",
        "softkeysactionbox/softkeysactionbox.qbs",
        "winintegration/winintegration.qbs",
    ]
}
