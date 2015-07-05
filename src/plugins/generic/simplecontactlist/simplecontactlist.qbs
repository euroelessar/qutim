import qbs.base 1.0
import "../GenericPlugin.qbs" as GenericPlugin

Project {
    GenericPlugin {
        sourcePath: "contactlist"

        Depends { name: "qutim-simplecontactlist" }
    }

    references: [
        "libqutim-simplecontactlist.qbs",
        "simplecontactdelegate/simplecontactdelegate.qbs",
        "simplecontactlist-headers.qbs",
        "widgets/simplecontactlistwidget/simplecontactlistwidget.qbs",
        "widgets/torycontactlistwidget/torycontactlistwidget.qbs"
    ]
}
