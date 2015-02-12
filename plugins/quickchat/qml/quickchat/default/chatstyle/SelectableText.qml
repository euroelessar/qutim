import QtQuick 2.3
import org.qutim.quickchat 0.4

TextEdit {
    id: textEdit

    readOnly: true
    selectByMouse: false
    selectByKeyboard: false
    activeFocusOnPress: false

    property SelectableMouseArea mouseArea

    Component.onCompleted: mouseArea.addItem(textEdit)
    Component.onDestruction: mouseArea.removeItem(textEdit)
}
