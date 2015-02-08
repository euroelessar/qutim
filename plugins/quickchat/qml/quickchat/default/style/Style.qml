import QtQuick 2.0
import org.qutim 0.4
import ".." as Parent

Parent.Style {
    id: style

    backgroundColor: "transparent"

    property Config _config: Config {
        id: config
        group: "contactList/style/default"

        property alias backgroundColor: style.backgroundColor
    }

    property ItemStyle account: ItemStyle {
        id: accountStyle
        group: config.group + "/account"

        rowHeight: 18
        selectedColor: "#a06495ed"
        font {
            fontPointSize: 10
        }
    }

    property ItemStyle group: ItemStyle {
        id: groupStyle
        group: config.group + "/group"

        rowHeight: 18
        backgroundColor: "white"
        selectedColor: "white"
        font {
            textColor: "white"
            highlightColor: "white"
            fontPointSize: 10
        }
        backgroundGradient: [
            { position: 0.0, color: Qt.lighter("#1959d1", 1.2) },
            { position: 1.0, color: "#1959d1" }
        ]
        selectedGradient: [
            { position: 0.0, color: "#1959d1" },
            { position: 1.0, color: Qt.darker("#1959d1", 1.2) }
        ]
    }

    property ItemStyle contact: ItemStyle {
        id: contactStyle
        group: config.group + "/contact"

        rowHeight: 28
        selectedColor: "#a06495ed"
        alternateColor: "#306495ed"
        font {
            fontPointSize: 12
        }

        property FontStyle subtitleFont: FontStyle {
            fontPointSize: 8
        }
    }

    property Component groupCanvasComponent: Canvas {
        id: groupCanvas

        readonly property real size: Math.round(height * 0.7)

        onPaint: {
            var ctx = groupCanvas.getContext('2d');

            ctx.save();

            ctx.fillStyle = group.font.textColor;

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
                text: model === null ? 'null' : model.id
                fontStyle: accountStyle.font
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
                sourceComponent: style.groupCanvasComponent
                width: height
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                    leftMargin: 2
                }
                rotation: model !== null && model.collapsed ? -90 : 0
            }

            Label {
                anchors {
                    left: image.right
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    leftMargin: 4
                }
                text: model !== null ? model.title + ' (' + model.onlineCount + '/' + model.totalCount + ')' : 'null'
                fontStyle: groupStyle.font
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
                fontStyle: contactStyle.font
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
                fontStyle: contactStyle.subtitleFont
            }
        }
    }
}
