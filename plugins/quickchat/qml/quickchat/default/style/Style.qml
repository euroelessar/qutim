import QtQuick 2.0
import ".." as Parent

Parent.Style {
    id: style

    property color backgroundColor: "transparent"

    property ItemStyle account: ItemStyle {
        source: 'Account.qml'
        rowSource: 'AccountRow.qml'
        rowHeight: 18
        selectedColor: "#a06495ed"
        property real fontPointSize: 10
    }

    property ItemStyle group: ItemStyle {
        source: 'Group.qml'
        rowSource: 'GroupRow.qml'
        rowHeight: 18
        backgroundColor: "white"
        selectedColor: "white"
        textColor: "white"
        highlightColor: "white"
        property real fontPointSize: 10
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
        source: 'Contact.qml'
        rowSource: 'ContactRow.qml'
        rowHeight: 28
        selectedColor: "#a06495ed"
        property real fontPointSize: 12
    }

    property Component groupTriangle: Canvas {
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

    rowAccountDelegate: account.rowSource
    rowGroupDelegate: group.rowSource
    rowContactDelegate: contact.rowSource
    itemAccountDelegate: account.source
    itemGroupDelegate: group.source
    itemContactDelegate: contact.source
}
