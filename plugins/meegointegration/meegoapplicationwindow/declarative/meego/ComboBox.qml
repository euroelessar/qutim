// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Rectangle {
    id: comboBox
    
    property Style platformStyle: ComboBoxStyle {}
    property alias model: selectionDialog.model
    property alias title: selectionDialog.titleText
    property alias currentIndex: selectionDialog.selectedIndex
    property variant currentItem: model.get(currentIndex)
    property alias pressed: mouseArea.pressed
    property variant items: []
    
    onItemsChanged: {
        comboBox.model.clear();
        for (var i = 0; i < items.length; ++i)
            comboBox.model.append({ name: items[i] });
    }
    
    function indexOf(name, def) {
        for (var i = 0; i < comboBox.model.count; ++i) {
            var item = comboBox.model.get(i);
            if (item.name === name)
                return i;
        }
        return def || -1;
    }
    
    width: parent.width
    height: wrapper.height + wrapper.anchors.topMargin + wrapper.anchors.bottomMargin
    color: pressed ? platformStyle.itemSelectedBackgroundColor : platformStyle.itemBackgroundColor
    
    Item {
        id: wrapper
        anchors.margins: 10
        anchors { top: comboBox.top; left: comboBox.left; right: comboBox.right }
        height: titleLabel.height + 5 + valueLabel.height
        Label {
            id: titleLabel
            anchors { left: wrapper.left; right: image.left; top: wrapper.top }
            text: title
            
            font.family: comboBox.platformStyle.fontFamily
            font.weight: comboBox.platformStyle.fontWeight
            font.pixelSize: comboBox.platformStyle.fontPixelSize
            font.capitalization: comboBox.platformStyle.fontCapitalization
            color: !enabled ? comboBox.platformStyle.disabledTextColor :
                   pressed ? comboBox.platformStyle.pressedTextColor :
                             comboBox.platformStyle.textColor;
        }
        Label {
            id: valueLabel
            anchors { left: wrapper.left; right: image.left; topMargin: 5; top: titleLabel.bottom }
            text: currentItem === undefined ? "" : currentItem.name
            
            font.family: comboBox.platformStyle.fontFamily
            font.weight: comboBox.platformStyle.subfontWeight
            font.pixelSize: comboBox.platformStyle.fontPixelSize
            font.capitalization: comboBox.platformStyle.fontCapitalization
            color: !enabled ? comboBox.platformStyle.disabledSubtextColor :
                   pressed ? comboBox.platformStyle.pressedSubtextColor :
                             comboBox.platformStyle.subtextColor;
        }
        Image {
            id: image
            anchors { right: wrapper.right; verticalCenter: wrapper.verticalCenter }
            source: comboBox.platformStyle.indicatorSource
        }
    }
    SelectionDialog {
        id: selectionDialog
    }
    
    MouseArea {
        id: mouseArea
        anchors {
            fill: parent
            rightMargin: (platformStyle.position != "horizontal-center"
                            && platformStyle.position != "horizontal-left") ? platformStyle.mouseMarginRight : 0
            leftMargin: (platformStyle.position != "horizontal-center"
                            && platformStyle.position != "horizontal-right") ? platformStyle.mouseMarginLeft : 0
            topMargin: (platformStyle.position != "vertical-center"
                            && platformStyle.position != "vertical-bottom") ? platformStyle.mouseMarginTop : 0
            bottomMargin: (platformStyle.position != "vertical-center"
                            && platformStyle.position != "vertical-top") ? platformStyle.mouseMarginBottom : 0
        }
        onClicked: selectionDialog.open()
    }
}
