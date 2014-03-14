import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.qutim 0.4

SettingsItem {
    id: root
    
    Emoticons {
        id: emoticons
    }
    
    function save() {
        var index = comboBox.currentIndex;
        emoticons.themeName = themesModel.get(index).text;
    }
    
    function load() {
        themesModel.clear();
        themesModel.append({ text: qsTr("No emoticons") })
        
        var index = 0;
        
        var themes = emoticons.themeList;
        themes.sort();
        for (var i = 0; i < themes.length; ++i) {
            if (themes[i] === emoticons.themeName)
                index = i + 1;
            
            themesModel.append({ text: themes[i] });
        }
        
        comboBox.currentIndex = index;
    }
    
    ListModel {
        id: themesModel
    }
    
    ColumnLayout {
        anchors.fill: parent
        
        ComboBox {
            id: comboBox
            Layout.fillWidth: true
            model: themesModel
            onCurrentIndexChanged: root.modify()
        }
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            GridView {
                EmoticonsTheme {
                    id: emoticonsTheme
                    themeName: comboBox.currentText
                }
                
                model: emoticonsTheme.emoticons
                delegate: AnimatedImage {
                    source: modelData.url
                }
            }
        }
    }
}
