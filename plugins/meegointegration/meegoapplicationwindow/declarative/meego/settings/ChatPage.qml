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
            var controller = chatView.controller;
            var style = root.style;
            var configPrefix = "style/";
            config.setValue(configPrefix + "showUserIcons", userIconsSwitch.checked);
            config.setValue(configPrefix + "showHeader", headerSwitch.checked);
            config.setValue(configPrefix + "name", styleBox.currentItem.name);
            configPrefix += styleBox.currentItem.name + "/";
            config.setValue(configPrefix + "variant", style.activeVariant);
            config.setValue(configPrefix + "fontFamily", controller.fontFamily);
            config.setValue(configPrefix + "fontSize", controller.fontSize);
        }
        function load() {
	    var style = root.style;
	    var styleName = config.value("style/name", "default");
	    userIconsSwitch.checked = config.value("style/showUserIcons", true);
            headerSwitch.checked = config.value("style/showHeader", true);
	    styleBox.items = themeManager.list("webkitstyle");
	    styleBox.currentIndex = styleBox.indexOf(styleName);
        }
    }
    Timer {
        interval: 100
        running: true
        repeat: false
        onTriggered: chatPreview.rebuildChat()
    }
    property QtObject style: chatView.controller.style
    ThemeManager {
        id: themeManager
    }
    ChatPreview {
        id: chatPreview
        function rebuildChat() {
            var controller = chatView.controller;
            console.log("rebuildChat", controller.webView, controller.session);
            controller.clearChat();
            var messages = chatPreview.messages;
            for (var i = 0; i < messages.length; ++i)
                controller.append(messages[i]);
        }
        function updateFont() {
            var fontFamily = fontFamiliesBox.currentItem.name;
            var fontSize = fontSizeField.value;
            chatView.controller.setDefaultFont(fontFamily, fontSize);
        }
    }
    Config {
        id: config
        path: "appearance/adiumChat"
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
            ChatView {
                id: chatView
                width: parent.width
                preview: true
                height: Math.min(root.width, root.height) * 0.75
                session: chatPreview.session
            }
            ComboBox {
                id: styleBox
                title: qsTr("Style")
                onCurrentItemChanged: {
                    var styleName = currentItem.name;
                    var configPrefix = "style/" + styleName + "/";
                    root.style.stylePath = themeManager.path("webkitstyle", styleName);
                    variantBox.items = root.style.variants;
                    var variant = config.value(configPrefix + "variant", root.style.activeVariant);
                    var fontFamily = config.value(configPrefix + "fontFamily", root.style.defaultFontFamily);
                    var fontSize = config.value(configPrefix + "fontSize", root.style.defaultFontSize);
                    variantBox.currentIndex = variantBox.indexOf(variant, 0);
                    fontFamiliesBox.currentIndex = fontFamiliesBox.indexOf(fontFamily, 0);
                    fontSizeField.text = fontSize;
                    chatPreview.rebuildChat();
                    chatPreview.updateFont();
                }
            }
            ComboBox {
                id: variantBox
                title: qsTr("Style variant")
                visible: model.count > 0
                onCurrentItemChanged: {
                    root.style.activeVariant = currentItem.name;
                    var script = root.style.scriptForChangingVariant();
                    chatView.controller.evaluateJavaScript(script);
                }
            }
            SwitchRow {
                id: userIconsSwitch
                text: qsTr("Show user icons")
                onCheckedChanged: {
                    root.style.showUserIcons = checked;
                    chatPreview.rebuildChat();
                }
            }
            SwitchRow {
                id: headerSwitch
                text: qsTr("Show header")
                onCheckedChanged: {
                    root.style.showHeader = checked;
                    chatPreview.rebuildChat();
                }
            }
            ComboBox {
                id: fontFamiliesBox
                title: qsTr("Font family")
                onCurrentItemChanged: chatPreview.updateFont()
            }
            Label {
                text: qsTr("Font size")
            }
            TextField {
                id: fontSizeField
                width: parent.width
                property int value: parseInt(fontSizeField.text)
                validator: IntValidator {
                    id: validator
                    bottom: 1
                    top: 120
                }
                inputMethodHints: Qt.ImhDigitsOnly
                onValueChanged: chatPreview.updateFont()
            }
        }
    }
    Component.onCompleted: {
        fontFamiliesBox.items = chatPreview.fontFamilies;
    }
}
