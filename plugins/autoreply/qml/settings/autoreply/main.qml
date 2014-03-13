import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.qutim 0.4

SettingsItem {
    id: root
    
    Config {
        id: config
        path: "autoreply"
    }
    
    function save() {
        var messages = [];
        for (var i = 0; i < messagesModel.count; ++i)
            messages.push(messagesModel.get(i).message);
        
        config.setValue("automatic", automaticEdit.checked);
        config.setValue("timeOut", timeOutEdit.value * 60);
        config.setValue("deltaTime", deltaTimeEdit.value * 60);
        config.setValue("message", messageEdit.text);
        config.setValue("messages", messages);
    }
    
    function load() {
        automaticEdit.checked = config.value("automatic", true);
        timeOutEdit.value = config.value("timeOut", 60 * 15) / 60;
        deltaTimeEdit.value = config.value("deltaTime", 60 * 15) / 60;
        messageEdit.text = config.value("message", "");
        var messages = config.value("messages", []);
        
        messagesModel.clear();
        for (var i = 0; i < messages.length; ++i)
            messagesModel.append({ message: messages[i] });
    }
    
    ListModel {
        id: messagesModel
    }
    
    GridLayout {
        anchors.fill: parent
        columns: 2
        
        CheckBox {
            id: automaticEdit
            text: qsTr("Enable autoreply by idle")
            Layout.columnSpan: 2
            onCheckedChanged: root.modify()
        }
        
        Label {
            text: qsTr("Idle timeout:")
            enabled: automaticEdit.checked
        }
        
        SpinBox {
            id: timeOutEdit
            suffix: qsTr(" min.")
            stepSize: 1
            minimumValue: 1
            maximumValue: 3600
            Layout.fillWidth: true
            enabled: automaticEdit.checked
            onValueChanged: root.modify()
        }
        
        Label {
            text: qsTr("Delta time between messages:")
            enabled: automaticEdit.checked
        }
        
        SpinBox {
            id: deltaTimeEdit
            suffix: qsTr(" min.")
            stepSize: 1
            minimumValue: 1
            maximumValue: 3600
            Layout.fillWidth: true
            onValueChanged: root.modify()
        }
        
        Item {
            Layout.columnSpan: 2
            Layout.fillHeight: true
            Layout.fillWidth: true
            
            TextArea {
                id: messageEdit
                anchors.fill: parent
                visible: automaticEdit.checked
                
                text: "Hello!"
                textFormat: TextEdit.PlainText
                enabled: automaticEdit.checked
                onTextChanged: root.modify()
            }
            
            GridLayout {
                anchors.fill: parent
                columns: 2
                visible: !automaticEdit.checked
                
                ScrollView {
                    Layout.columnSpan: 2
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    
                    ListView {
                        anchors.fill: parent
                        model: messagesModel
                        delegate: RowLayout {
                            width: parent.width
                            height: 100
                            TextArea {
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                                
                                text: message
                                onTextChanged: {
                                    messagesModel.setProperty(index, "message", text);
                                    root.modify()
                                }
                            }
                            Button {
                                text: qsTr("Remove")
                                onClicked: {
                                    messagesModel.remove(index);
                                    root.modify();
                                }
                            }
                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
                Button {
                    text: qsTr("Add answer")
                    onClicked: {
                        messagesModel.append({ message: qsTr("Enter your text here") });
                        root.modify();
                    }
                }
            }
        }
    }
}
