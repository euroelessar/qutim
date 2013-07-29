import qbs.base 1.0
import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

Project {
    UreenPlugin {
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
