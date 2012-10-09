import QtQuick 1.1
import QtDesktop 0.1
import org.qutim.plugman 0.3

Item {
    id: root
    ListView {
        id: listView
        anchors.margins: 4
        spacing: 4
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            right: groupBox.left
        }
        model: PackageModel {
            id: packageModel
            categories: [ "Emoticon Theme" ]
            path: "emoticons"
            sortMode: orderByColumn.checkedButton.value
        }
        delegate: Item {
            width: listView.width
            height: 108
            anchors.margins: 4
           // color: Qt.rgba(Math.random(), Math.random(), Math.random())
            Image {
                id: itemImage
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.margins: 4
                source: imageSource
                width: 100
                height: 54
            }
            Label {
                id: itemTitle
                anchors.left: itemImage.right
                anchors.top: parent.top
                anchors.right: itemButton.left
                anchors.margins: 4
                font.bold: true
                text: title
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                maximumLineCount: 1
            }
            Label {
                id: itemDescription
                anchors.left: itemImage.right
                anchors.top: itemTitle.bottom
                anchors.bottom: parent.bottom
                anchors.margins: 4
                anchors.right: itemButton.left
                text: description
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                maximumLineCount: 6
                TooltipArea {
                    id: tooltip
                    anchors.fill: parent
                    text: description
                }
            }
            Button {
                id: itemButton
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.margins: 4
                text: suggestData(status).text
                iconSource: suggestData(status).iconSource
                enabled: status !== PackageModel.Installing && status !== PackageModel.Updating
                function suggestData(state) {
                    var text;
                    var iconSource = "";
                    switch (state) {
                    default:
                        text = "Invalid - " + state;
                        break;
                    case PackageModel.Installable:
                        text = qsTr("Install");
//                        iconSource = "image://desktoptheme/dialog-ok"
                        break;
                    case PackageModel.Updateable:
                        text = qsTr("Update");
//                        iconSource = "image://desktoptheme/system-software-update"
                        break;
                    case PackageModel.Installing:
                        text = qsTr("Installing");
//                        iconSource = "image://desktoptheme/dialog-ok"
                        break;
                    case PackageModel.Updating:
                        text = qsTr("Updating");
//                        iconSource = "image://desktoptheme/dialog-ok"
                        break;
                    case PackageModel.Installed:
                        text = qsTr("Uninstall");
//                        iconSource = "image://desktoptheme/edit-delete"
                        break;
                    }
                    return { text: text, iconSource: iconSource };
                }
                onClicked: {
                    if (status == PackageModel.Installable || status == PackageModel.Updateable)
                        packageModel.install(index);
                    else if (status == PackageModel.Installed)
                        packageModel.remove(index);
                }
            }
        }
        clip: true
    }
    GroupBox {
        id: groupBox
        title: qsTr("Order by:")
        adjustToContentSize: true
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
            rightMargin: 8
            bottomMargin: 8
        }

        ButtonColumn {
            id: orderByColumn
            exclusive: true
            RadioButton {
                text: qsTr("Newest")
                checked: true
                property int value: PackageModel.Newest
//                    KeyNavigation.tab: area
//                    KeyNavigation.backtab: r1
            }
            RadioButton {
                text: qsTr("Rating")
                property int value: PackageModel.Rating
            }
            RadioButton {
                text: qsTr("Most downloads")
                property int value: PackageModel.Downloads
            }
//            RadioButton {
//                text: qsTr("Installed")
//                property int value: PackageModel.Newest
//            }
        }
    }
}
