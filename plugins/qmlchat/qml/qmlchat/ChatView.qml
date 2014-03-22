import QtQuick 2.2

Item {
    id: root
    property Item currentItem

    function setItem(item) {
        if (currentItem) {
            currentItem.anchors.fill = undefined;
            currentItem.parent = null;
        }

        currentItem = item;
        if (currentItem) {
            currentItem.parent = root;
            currentItem.anchors.fill = Qt.binding(function () { return root });
        }
    }
}
