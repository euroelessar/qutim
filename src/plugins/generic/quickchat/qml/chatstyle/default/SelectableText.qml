import QtQuick 2.3
import org.qutim.quickchat 0.4

TextEdit {
    id: textEdit

    readOnly: true
    selectByMouse: false
    selectByKeyboard: false
    activeFocusOnPress: false

    property SelectableMouseArea __selectableMouseArea

    Component.onCompleted: {
        var current = parent;
        while (current) {
            if (current.__selectableMouseArea !== undefined) {
                __selectableMouseArea = current.__selectableMouseArea;
                break;
            }
            current = current.parent;
        }

        if (__selectableMouseArea)
            __selectableMouseArea.addItem(textEdit);
    }

    Component.onDestruction: {
        if (__selectableMouseArea)
            __selectableMouseArea.removeItem(textEdit);
    }
}
