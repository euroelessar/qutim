import QtQuick 2.0
import ".." as Parent

Parent.Style {
    id: style

    property color backgroundColor: "transparent"

    property ItemStyle account: ItemStyle {
        id: accountStyle
        source: 'Account.qml'
        rowHeight: 18
        selectedColor: "#a06495ed"
        fontPointSize: 10
    }

    property ItemStyle group: ItemStyle {
        id: groupStyle
        source: 'Group.qml'
        rowHeight: 18
        backgroundColor: "white"
        selectedColor: "white"
        textColor: "white"
        highlightColor: "white"
        fontPointSize: 10
        backgroundGradient: Gradient {
            GradientStop { position: 0.0; color: Qt.lighter("#1959d1", 1.2) }
            GradientStop { position: 1.0; color: "#1959d1" }
        }
        selectedGradient: Gradient {
            GradientStop { position: 0.0; color: "#1959d1" }
            GradientStop { position: 1.0; color: Qt.darker("#1959d1", 1.2) }
        }
    }

    property ItemStyle contact: ItemStyle {
        id: contactStyle
        source: 'Contact.qml'
        rowHeight: 28
        selectedColor: "#a06495ed"
        fontPointSize: 12
    }

    property Component groupCanvasComponent: Canvas {
        id: groupCanvas

        readonly property real size: Math.round(height * 0.7)

        onPaint: {
            var ctx = groupCanvas.getContext('2d');

            ctx.save();

            ctx.fillStyle = group.textColor;

            var middle = Math.round(height / 2);
            var halfSize = Math.round(size / 2);
            var delta = Math.round(size / 3);

            ctx.beginPath();
            ctx.moveTo(middle - halfSize, middle - delta);
            ctx.lineTo(middle + halfSize, middle - delta);
            ctx.lineTo(middle, middle + delta);
            ctx.closePath();

            ctx.fill();

            ctx.restore();
        }
    }

    accountDelegate: BaseRow {
        id: accountItem
        height: itemStyle.rowHeight

        itemStyle: accountStyle

        Item {
            anchors.fill: parent
            anchors.margins: accountStyle.rowMargin

            Label {
                anchors.fill: parent
                text: model === null ? 'null' : model.title
                itemStyle: accountStyle
            }
        }
    }

    groupDelegate: BaseRow {
        id: groupItem
        height: itemStyle.rowHeight

        itemStyle: groupStyle

        Item {
            anchors.fill: parent
            anchors.margins: groupStyle.rowMargin

            Loader {
                id: image
    //            sourceComponent: viewStyle.groupTriangle
                width: height
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                    leftMargin: 2
                }
            }

            Label {
                anchors {
                    left: image.right
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    leftMargin: 4
                }
                text: model === null ? 'null' : model.title
                itemStyle: groupStyle
            }
        }
    }

    contactDelegate: BaseRow {
        id: contactItem
        height: itemStyle.rowHeight

        itemStyle: contactStyle

        Item {
            anchors.fill: parent
            anchors.margins: itemStyle.rowMargin

            Image {
                id: avatar
                width: height
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                source: model && model.iconSource && height > 0 ? model.iconSource + '&size=' + height : ''
            }

            Label {
                id: titleText
                anchors {
                    left: avatar.right
                    right: parent.right
                    top: subtitleText.visible ? parent.top : undefined
                    verticalCenter: subtitleText.visible ? undefined : parent.verticalCenter
                    leftMargin: 4
                }
                text: model === null ? 'null' : model.title
                itemStyle: contactStyle
            }

            Label {
                id: subtitleText
                anchors {
                    left: avatar.right
                    right: parent.right
                    top: titleText.bottom
                    bottom: parent.bottom
                    leftMargin: 4
                }
                visible: text.length > 0
                text: model === null || model.contact === undefined ? '' : model.contact.status.text
                itemStyle: contactStyle
                font.pointSize: titleText.font.pointSize - 4
            }
        }
    }
}
