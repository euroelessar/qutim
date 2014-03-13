import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.qutim 0.4

SettingsItem {
    id: root
    
    Config {
        id: config
        group: "urlPreview"
    }
    
    function save() {
        
        config.setValue("maxFileSize", maxFileSizeEdit.checked);
        config.setValue("maxWidth", maxWidthEdit.value);
        config.setValue("maxHeight", maxHeightEdit.value);
        config.setValue("youtubePreview", youtubePreviewEdit.checked);
        config.setValue("imagesPreview", imagesPreviewEdit.checked);
        config.setValue("HTML5Audio", html5AudioEdit.checked);
        config.setValue("HTML5Video", html5VideoEdit.checked);
        config.setValue("yandexRCA", yandexRCAEdit.checked);
        config.setValue("exceptionList", exceptionListEdit.text.split(';'));
    }
    
    function load() {
        maxFileSizeEdit.value = config.value("maxFileSize", 100000);
        maxWidthEdit.value = config.value("maxWidth", 800);
        maxHeightEdit.value = config.value("maxHeight", 600);
        youtubePreviewEdit.checked = config.value("youtubePreview", true);
        imagesPreviewEdit.checked = config.value("imagesPreview", true);
        html5AudioEdit.checked = config.value("HTML5Audio", true);
        html5VideoEdit.checked = config.value("HTML5Video", true);
        yandexRCAEdit.checked = config.value("yandexRCA", true);
        exceptionListEdit.text = config.value("exceptionList", []).join(';');
    }
    GridLayout {
        anchors.fill: parent
        columns: 2
        Label {
            text: "Max. file size:"
        }
        SpinBox {
            id: maxFileSizeEdit
            suffix: qsTr(" kb.")
            stepSize: 10
            minimumValue: 100
            maximumValue: 1000000
            Layout.fillWidth: true
            enabled: true
            onValueChanged: root.modify()
        }
        Label {
            text: "Max. width:"
        }
        SpinBox {
            id: maxWidthEdit
            suffix: qsTr(" px.")
            stepSize: 10
            minimumValue: 50
            maximumValue: 2000
            Layout.fillWidth: true
            enabled: true
            onValueChanged: root.modify()
        }
        Label {
            text: "Max. height:"
        }
        SpinBox {
            id: maxHeightEdit
            suffix: qsTr(" px.")
            stepSize: 10
            minimumValue: 50
            maximumValue: 2000
            Layout.fillWidth: true
            enabled: true
            onValueChanged: root.modify()
        }
        CheckBox {
            id: youtubePreviewEdit
            text: qsTr("Enable youtube preview")
            Layout.columnSpan: 2
            onCheckedChanged: root.modify()
        }
        CheckBox {
            id: imagesPreviewEdit
            text: qsTr("Enable images preview")
            Layout.columnSpan: 2
            onCheckedChanged: root.modify()
        }
        CheckBox {
            id: html5AudioEdit
            text: qsTr("Enable HTML5 Audio")
            Layout.columnSpan: 2
            onCheckedChanged: root.modify()
        }
        CheckBox {
            id: html5VideoEdit
            text: qsTr("Enable HTML5 Video")
            Layout.columnSpan: 2
            onCheckedChanged: root.modify()
        }
        CheckBox {
            id: yandexRCAEdit
            text: qsTr("Enable RCA preview")
            Layout.columnSpan: 2
            onCheckedChanged: root.modify()
        }
        Label {
            text: "Please divide your words with ';'"
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }
        TextArea {
            id: exceptionListEdit
            Layout.columnSpan: 2
            Layout.fillWidth: true
            onTextChanged: root.modify()
        }
        Item {
            Layout.fillHeight: true
        }
    }
}

