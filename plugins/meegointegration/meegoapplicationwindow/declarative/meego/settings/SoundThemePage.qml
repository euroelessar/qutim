// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import ".."

SettingsItemPage {
    id: root
    impl: QtObject {
        function save() {
            soundTheme.setCurrentName(themeBox.currentItem.name);
        }
        function load() {
            themeBox.model.clear();
            var themeList = soundTheme.themeList();
            themeBox.model.append({ name: qsTr("No sound") });
            themeBox.currentIndex = 0;
            for (var i = 0; i < themeList.length; ++i) {
                themeBox.model.append({ name: themeList[i] });
                if (themeList[i] === soundTheme.currentName())
                    themeBox.currentIndex = i + 1;
            }
        }
    }
    SoundTheme {
        id: soundTheme
        name: themeBox.currentItem === null ? "" : themeBox.currentItem.name
    }
    
    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: column.width
        contentHeight: column.height
        Column {
            id: column
            width: flickable.width
            spacing: 10
            ComboBox {
                id: themeBox
                title: qsTr("Theme name")
            }
            
            Repeater {
                model: Notification.LastType + 1
                
                Item {
                    id: soundItem
                    width: parent.width
                    height: wrapper.height + wrapper.anchors.topMargin + wrapper.anchors.bottomMargin
                    
                    Item {
                        id: wrapper
                        anchors.margins: 10
                        anchors { top: soundItem.top; left: soundItem.left; right: soundItem.right }
                        height: titleLabel.height
                        Label {
                            id: titleLabel
                            anchors { left: wrapper.left; right: image.left; top: wrapper.top }
                            text: soundTheme.typeText(index)
                        }
                        ToolIcon {
                            id: image
                            anchors { right: wrapper.right; verticalCenter: wrapper.verticalCenter }
                            iconId: "toolbar-mediacontrol-play" + (enabled ? "" : "-dimmed")
                            enabled: soundTheme.name && (soundTheme.path(index) !== "")
                            onClicked: soundTheme.play(index)
                        }
                    }   
                }
            }
        }
    }
}
