import qbs.base 1.0
import "../../../../plugins/UreenPlugin.qbs" as UreenPlugin

UreenPlugin {
    sourcePath: "contactlist"

    Depends { name: "libqutim-simplecontactlist" }
}
