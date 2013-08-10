import QtQuick 1.1
import qutIM 0.3

Rectangle {
    id: main

    width: 300
    height: childrenRect.height
    radius: 10
    smooth: true
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#6495ED" }
        GradientStop { position: 0.33; color: "#191970" }
        GradientStop { position: 1.0; color: "#000080" }
    }

    PopupAttributes {
        id: attributes
        property int trimLength: 80

        frameStyle: PopupAttributes.ToolTip
    }

    Image {
        id: image
        source: "images/qutim.svg"
        width: 30
        fillMode: Image.PreserveAspectFit

        anchors {
            top: main.top
            left: main.left
            topMargin: 15
            leftMargin: 5
        }
    }

    Text {
        id: subject
        //text: "Title"

        anchors {
            top: main.top
            left: image.right
            right: main.right
            topMargin: 5
            leftMargin: 5
            rightMargin: 5
        }

        elide: Text.ElideMiddle
        color: "white"
        font.bold: true
        style: Text.Sunken
        styleColor: "black"
        font.pixelSize: body.font.pixelSize + 2
    }

    Text {
        id: body
        wrapMode: Text.Wrap

        anchors {
            top: subject.bottom
            left: subject.left
            right: subject.right
        }

        color: "white"
        style: Text.Sunken
        styleColor: "black"
    }

    ListView {
        id: actions

        z: 10
        height: 30
        orientation: ListView.Horizontal
        spacing: 10

        anchors {
            top: body.bottom
            left: body.left
            leftMargin: 90
            right: body.right
            rightMargin: 10
        }
        delegate: Text {
            id: actionDelegate
            text: model.modelData.text

            color: "#D3D3D3"
            style: Text.Sunken
            styleColor: "black"
            font.pointSize: 9
            //			font.italic: true
            font.underline: false

            MouseArea {
                id: actionArea
                anchors.fill: parent

                hoverEnabled: true
                acceptedButtons: Qt.LeftButton

                onClicked: {
                    console.log("Action: " + model.modelData);
                    model.modelData.trigger();
                    mouse.accepted = true;
                }
            }

            states: [
                State {
                    name: "hovered"
                    when: actionArea.containsMouse
                    PropertyChanges {
                        target: actionDelegate
                        font.underline: true
                    }
                },
                State {
                    name: "leaved"
                    when: !actionArea.containsMouse
                    PropertyChanges {
                        target: actionDelegate
                        font.underline: false
                    }
                },
                State {
                    name: "pressed"
                    when: actionArea.pressed
                    PropertyChanges {
                        target: actionDelegate
                        font.underline: true
                        font.bold: true
                    }
                }
            ]
        }
    }

    MouseArea {
        id: acceptIgnoreArea
        anchors.fill: main
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            console.log("Clicked");
            if (mouse.button === Qt.RightButton)
                popup.ignore();
            else
                popup.accept();
        }
    }

    Connections {
        target: popup
        onNotifyAdded: {
            var str = body.text;
            if (str.length && notify.text.length && str.length < attributes.trimLength)
                str = str + "<br /> ";
            str = str + notify.text;
            //trim
            if (str.length > attributes.trimLength)
                str = str.substring(0, attributes.trimLength - 3) + "...";
            body.text = str;

            subject.text = notify.title;
            actions.model = notify.actions;

            if (notify.avatar)
                image.source = notify.avatar;
        }
    }
}

