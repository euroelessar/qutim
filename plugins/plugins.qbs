import qbs.base 1.0

Project {
    name: "Plugins"

    references: [
        "adiumwebview/adiumwebview.qbs",
        "dbusnotifications/dbusnotifications.qbs",
        "antispam/antispam.qbs",
        "autopaster/autopaster.qbs",
        "autoreply/autoreply.qbs"
    ]
}
