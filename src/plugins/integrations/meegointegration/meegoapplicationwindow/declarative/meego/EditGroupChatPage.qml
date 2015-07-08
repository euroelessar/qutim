// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import org.qutim 0.3
import "constants.js" as UI

Page {
    id: root
    anchors.margins: 10
    property QtObject wrapper
    
    Connections {
        target: root.wrapper
        onBookmarksChanged: root.rebuildComboBox()
    }
    function rebuildComboBox() {
        console.debug("rebuildComboBox");
        bookmarksBox.model.clear();
        bookmarksBox.model.append({
                                  "name": qsTr("New bookmark"),
                                  "item": root.wrapper.fields()
                              });
        bookmarksBox.currentIndex = 0;
        var bookmarks = root.wrapper.bookmarks;
        bookmarks.concat(root.wrapper.recent);
        for (var i = 0; i < bookmarks.length; ++i) {
            var bookmark = bookmarks[i];
            bookmark.name = bookmark.title;
            bookmarksBox.model.append(bookmark);
        }
    }

    FlickableColumn {
        anchors.fill: parent
        spacing: 10
        ComboBox {
            id: bookmarksBox
            title: qsTr("Choose bookmark")
        }

        DataForm {
            id: dataForm
            width: parent.width
            item: bookmarksBox.currentItem.item
        }
    }
    
    tools: ToolBarLayout {
		id: toolBarLayout
		ToolIcon {
			visible: true
			platformIconId: "toolbar-previous"
			onClicked: pageStack.pop()
		}
        ToolButton {
            visible: true
            text: qsTr("Remove")
            enabled: bookmarksBox.currentIndex > 0
            onClicked: root.wrapper.remove(bookmarksBox.currentItem.item)
        }
        ToolButton {
            visible: true
            text: qsTr("Save")
            onClicked: root.wrapper.save(dataForm.item, bookmarksBox.currentItem.item)
        }
	}
    Component.onCompleted: root.rebuildComboBox()
}
