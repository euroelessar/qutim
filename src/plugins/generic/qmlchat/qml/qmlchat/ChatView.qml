import QtQuick 2.2

Item {
    id: root
    property Item currentItem

    function setItem(item) {
        console.log('item: ', currentItem, item)

        if (currentItem) {
            currentItem.anchors.fill = undefined;
            currentItem.visible = false;
        }

        currentItem = item;
        if (currentItem) {
            currentItem.parent = root;
            currentItem.anchors.fill = Qt.binding(function () { return root });
            currentItem.visible = true;
        }
    }
}
