import QtQuick 1.0
import com.nokia.meego 1.0
import com.nokia.extras 1.0

ToolBarLayout {
    id: toolBarLayout
    property Item page
    property bool acceptableInput: true

    ToolIcon {
        id: toolIcon
        visible: true
        enabled: toolBarLayout.acceptableInput
        platformIconId: "toolbar-previous" + (enabled ? "" : "-dimmed")
        onClicked: {
            toolBarLayout.page.impl.save();
            toolBarLayout.page.saved();
            toolBarLayout.page.pageStack.pop();
        }
    }
}
