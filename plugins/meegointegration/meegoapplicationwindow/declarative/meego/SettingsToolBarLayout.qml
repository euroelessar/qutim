import QtQuick 1.0
import com.nokia.meego 1.0
import com.nokia.extras 1.0

ToolBarLayout {
    id: toolBarLayout
    property Item page

    ToolIcon {
        id: toolIcon
        visible: true
        platformIconId: "toolbar-previous"
        onClicked: {
            toolBarLayout.page.impl.save();
            toolBarLayout.page.saved();
            toolBarLayout.page.pageStack.pop();
        }
    }
}
