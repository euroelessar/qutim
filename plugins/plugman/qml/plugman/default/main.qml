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
            spacing: 5

            TextField {
                id: searchField

                anchors.verticalCenter: parent.verticalCenter
                placeholderText: qsTr("Search")
                width: 200
            }

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
        }
    }

    ListView {
        id: listView
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
            filter: searchField.text
        }
        delegate: ItemDelegate {}
        clip: true
    }

    Balloon {
        width: 75
        height: 75
        radius: 10

        anchors.centerIn: parent
        opacity: packageModel.loading

        BusyIndicator {
            running: parent.visible
            anchors.centerIn: parent
            inverted: true

            implicitWidth: 64
            implicitHeight: 64
        }

    }
}
