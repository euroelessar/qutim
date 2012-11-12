import QtQuick 1.0

Rectangle {
    id: root

    property bool fullScreen: false

    width: 400
    height: 600

    PageHeader {
        id: header
        text: qsTr("%1's photoalbum").arg(contact.name)
    }

    Component {
        id: delegate
        Package {
            Item {
                id: listItem
                width: photoListView.width
                height: photoListView.height
                Package.name: 'list'
            }
            Item {
                id: gridItem
                width: photoGridView.cellWidth - photoGridView.spacing
                height: photoGridView.cellHeight - photoGridView.spacing
                Package.name: 'grid'
            }

            Image {
                id: wrapper
                source: src_big
                fillMode: Image.PreserveAspectCrop
                smooth: true
                clip: true

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        photoListView.currentIndex = index
                        //if (wrapper.state === "inList")
                        //    wrapper.state = "inGrid"
                        //else
                        //    wrapper.state = "inList"
                        fullScreen = !fullScreen
                    }
                }

                state: 'inGrid'
                states: [
                    State {
                        when: fullScreen
                        name: 'inList'
                        ParentChange {
                            target: wrapper
                            parent: listItem
                            width: listItem.width
                            height: listItem.height
                        }
                        //PropertyChanges { target: wrapper; clip: false }
                    },
                    State {
                        when: !fullScreen
                        name: 'inGrid'
                        ParentChange {
                            target: wrapper; parent: gridItem
                            x: 0; y: 0; width: gridItem.width; height: gridItem.height
                        }
                    }
                ]

                transitions: [
                    Transition {
                        ParentAnimation {
                            //target: wrapper; via: foreground
                            NumberAnimation { properties: 'x,y,width,height'; duration: 300 }
                        }
                    }
                ]
            }
        }
    }

    VisualDataModel {
        id: dataModel
        delegate: delegate
        model: photoModel
    }

    Item {
        id: foreground
        anchors.fill: parent
    }

    GridView {
        id: photoGridView

        onCurrentIndexChanged: positionViewAtIndex(currentIndex, GridView.Contain)

        visible: !fullScreen

        property int spacing: 6

        model: dataModel.parts.grid
        currentIndex: photoListView.currentIndex

        cellWidth: 150
        cellHeight: 150

        anchors {
            top: header.bottom
            left: root.left
            right: root.right
            bottom: root.bottom
            margins: 6
        }

        ScrollDecorator {
            flickableItem: photoGridView
        }
    }

    ListView {
        id: photoListView


        onCurrentIndexChanged: positionViewAtIndex(currentIndex, ListView.Contain)

        orientation: ListView.Horizontal
        snapMode: ListView.SnapToItem
        visible: fullScreen

        anchors {
            top: header.bottom
            left: root.left
            right: root.right
            bottom: root.bottom
            margins: 0
        }

        model: dataModel.parts.list
        currentIndex: photoGridView.currentIndex
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
}
