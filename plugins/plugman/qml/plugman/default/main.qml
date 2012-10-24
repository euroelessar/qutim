import QtQuick 1.1
import QtDesktop 0.1
import org.qutim.plugman 0.3

Item { //TODO use Window or Dialog with qml desktop components viewer
    id: root

    Header {
        id: header
        width: parent.width
        height: 32

        Row {
            anchors.fill: parent
            anchors.rightMargin: 5

            layoutDirection: Qt.RightToLeft

            ComboBox {
                id: orderByBox

                property int sortOrder: orderByModel.get(selectedIndex).value

                width: 200

                tooltip: qsTr("Order By")
                anchors.verticalCenter: parent.verticalCenter

                //HACK no way to translate this
                model: ListModel {
                    id: orderByModel
                    ListElement {
                        text: "Newest"
                        value: PackageModel.Newest
                    }
                    ListElement {
                        text: "Rating"
                        value: PackageModel.Rating
                    }
                    ListElement {
                        text: "Most downloads"
                        value: PackageModel.Downloads
                    }
                }
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Order by: ")
            }


            BusyIndicator {

                anchors.left: parent.left
                anchors.leftMargin: 15
                anchors.verticalCenter: parent.verticalCenter
                visible: packageModel.loading
            }
        }
    }

    ListView {
        id: listView
        anchors.margins: 4
        spacing: 4
        anchors {
            left: parent.left
            top: header.bottom
            bottom: parent.bottom
            right: parent.right
        }
        model: PackageModel {
            id: packageModel
            categories: [ "Emoticon Theme" ]
            path: "emoticons"
            sortMode: orderByBox.sortOrder
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
}
