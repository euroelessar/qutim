import QtQuick 1.0

Rectangle {
    id: root
    width: 400
    height: 600

    PageHeader {
        id: header
        text: qsTr("%1's photoalbum").arg(contact.name)
    }

    GridView {
        id: photoView

        model: photoModel

        cellWidth: 106
        cellHeight: 106

        clip: true
        anchors {
            top: header.bottom
            left: root.left
            right: root.right
            bottom: root.bottom
            margins: 6
        }

        delegate: Image {
            source: src
            width: 100
            height: 100
            fillMode: Image.PreserveAspectCrop
            clip: true

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    //simple hack
                    var photos = []
                    for (var i = 0; i !== page.model.count; i++)
                        photos.push(page.model.get(i))

                    var properties = {
                        "model" : photos,
                        "currentIndex" : index
                    }
                }
            }
        }
    }

    PhotoModel {
        id: photoModel

        Component.onCompleted: {
            getAll(contact.id)
        }
    }

    Connections {
        target: client

        onOnlineChanged: {
            if (client.online)
                photoModel.getAll(contact.id)
        }
    }

    ScrollDecorator {
        flickableItem: photoView
    }
}
