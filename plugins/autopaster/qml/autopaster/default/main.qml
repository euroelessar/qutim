import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import org.qutim 0.4
import org.qutim.autopaster 0.4

Dialog {
    id: root
    width: 450
    height: 150

    title: qsTr("Autopaster")
    standardButtons: StandardButton.Ok | StandardButton.Cancel

    Plugin {
        id: autopaster
        name: "autopaster"
    }

    readonly property QtObject handler: autopaster.object.handler

    Connections {
        target: root.handler
        onMessageReceived: root.visible = true
    }

    GridLayout {
        anchors.fill: parent
        columns: 2

        Label {
            text: qsTr("Pastebins: ")
        }

        ComboBox {
            id: pasterItems
            Layout.fillWidth: true
            textRole: "text"
            model: ListModel {
                id: pastersModel
            }
        }

        Label {
            text: qsTr("Languages: ")
        }

        ComboBox {
            id: syntaxItems
            Layout.fillWidth: true
            textRole: "text"
            model: ListModel {
                id: syntaxesModel
            }
        }
    }

    onButtonClicked: {
        if (clickedButton === StandardButton.Ok) {
            var paster = pastersModel.get(pasterItems.currentIndex).value;
            var syntax = syntaxesModel.get(syntaxItems.currentIndex).value;
            root.handler.upload(paster, syntax);
        } else {
            root.handler.cancel();
        }
    }

    onVisibleChanged: {
        if (visible)
            pasterItems.currentIndex = root.handler.currentPasterIndex
    }

    Component.onCompleted: {
        var i;

        var pasters = root.handler.pasters;
        for (i = 0; i < pasters.length; ++i) {
            pastersModel.append({
                text: pasters[i],
                value: pasters[i]
            });
        }

        var syntaxes = root.handler.syntaxes;
        for (i = 0; i < syntaxes.length; ++i) {
            syntaxesModel.append(syntaxes[i]);
        }

        console.log(JSON.stringify(pasters))
        console.log(JSON.stringify(syntaxes))
    }
}
