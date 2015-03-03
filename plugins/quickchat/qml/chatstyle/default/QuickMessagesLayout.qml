import QtQuick 2.3
import org.qutim.quickchat 0.4

Item {
    id: root

    property Component messageComponent
    property Component actionComponent
    property Component serviceComponent
    property QtObject session
    property var items: []
    property var itemMessages: []
    property var messageReceiptes: { return {}; }
    property var itemById: { return {}; }
    property SelectableMouseArea __selectableMouseArea: mouseArea
    property bool doNotRelayout: false

    function updateHover() {
        mouseArea.updateHover();
    }

    function copy() {
        mouseArea.copy();
    }

    Connections {
        target: session
        onReceivedMessageReceipt: {
            messageReceiptes[id] = success;

            if (!itemById.hasOwnProperty(id))
                return;
            var item = itemById[id];
            if (!item.hasOwnProperty('handleReceipt'))
                return;
            item.handleReceipt(id, success);
        }
        onClearRequested: {
            var oldItems = items;

            messageReceiptes = {};
            itemById = {};
            itemMessages = [];
            items = [];

            for (var i = 1; i <= oldItems.length; ++i) {
                var currentItem = i < oldItems.length ? oldItems[i] : undefined;
                var previousItem = oldItems[i - 1];
                if (previousItem !== currentItem) {
                    previousItem.parent = null;
                    previousItem.visible = false;
                    previousItem.destroy();
                }
            }

            doLayout();
        }
    }

    function appendMessage(message) {
        var index = findIndex(itemMessages, message);
        doNotRelayout = true;

        items.splice(index, 0, undefined);
        itemMessages.splice(index, 0, message);

        if (index > 0 && isContentSimiliar(itemMessages[index - 1], message)) {
            items[index] = items[index - 1];
            insertMessage(items[index], message);
        } else if (index + 1 < itemMessages.length && isContentSimiliar(message, itemMessages[index + 1])) {
            items[index] = items[index + 1];
            insertMessage(items[index], message);
        } else {
            items[index] = createItem([message]);

            if (index > 0 && index + 1 < items.length && items[index - 1] === items[index + 1]) {
                var messages = items[index - 1].messages;
                var messageIndex = messages.indexOf(itemMessages[index + 1]);
                var firstMessages = messages.slice(0, messageIndex);
                var secondMessages = messages.slice(messageIndex);
                items[index - 1].messages = firstMessages;
                items[index + 1] = createItem(secondMessages);
                for (var i = 1; i < secondMessages.length; ++i)
                    items[index + 1 + i] = items[index + 1];
            }
        }

        itemById[message.id] = items[index];

        doNotRelayout = false;
        doLayout();
    }

    function findIndex(messages, message) {
        var index = 0;
        for (index = 0; index < messages.length; ++index) {
            if (message.time.getTime() < messages[index].time.getTime()) {
                break;
            }
        }
        return index;
    }

    function insertMessage(item, message) {
        var messages = item.messages.slice();
        var index = findIndex(messages, message);
        messages.splice(index, 0, message);
        item.messages = messages;
    }

    function isContentSimiliar(first, second) {
        return !first.property("service", false)
            && !second.property("service", false)
            && first.isSimiliar(second);
    }

    function createItem(messages) {
        var message = messages[0];
        var properties = {
            messages: messages,
            session: Qt.binding(function () { return root.session; }),
            width: Qt.binding(function () { return root.width; }),
        };
        var component = messageComponent;
        if (message.action && actionComponent)
            component = actionComponent;
        if (message.property("service", false) && serviceComponent)
            component = serviceComponent;
        var item = component.createObject(root, properties);

        item.heightChanged.connect(function () {
            root.onItemHeightChanged(item);
        });

        return item;
    }

    function doLayout() {
        if (doNotRelayout)
            return;

        var y = 0;
        for (var i = 0; i < items.length; ++i) {
            var item = items[i];
            if (i > 0 && item === items[i - 1])
                continue;

            item.y = y;
            y += item.height;
        }
        root.height = y;
    }

    function onItemHeightChanged(item) {
        doLayout();
    }

    SelectableMouseArea {
        id: mouseArea
        anchors.fill: root
        z: 10
        hoverEnabled: true
        onLinkActivated: Qt.openUrlExternally(link)
    }
}
