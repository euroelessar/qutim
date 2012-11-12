import QtQuick 1.1
import QtDesktop 0.1
import org.qutim.plugman 0.3

Rectangle {
    id: root

    height: 140
    width: parent ? parent.width : 300
    color: index % 2 ? syspal.alternateBase : "transparent"

    SystemPalette { id: syspal }

    Image {
        id: preview

        width: 100
        height: Math.min(sourceSize.height, 100)

        source: imageSource ? imageSource : "image://qutim/utilities-file-archiver"
        fillMode: Image.PreserveAspectFit
        clip: true
        smooth: true

        anchors {
            left: parent.left
            leftMargin: 5
            top: column.top
        }

        BusyIndicator {
            anchors.centerIn: parent
            size: "large"
            visible: preview.status !== Image.Ready
        }
    }

    Column {
        id: column

        spacing: 2

        anchors {
            left: preview.right
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            leftMargin: 10
            rightMargin: 10
            topMargin: 10
        }

        Text {
            id: titleLabel
            width: parent.width
            font.bold: true
            text: title
            elide: Text.ElideRight
            wrapMode: Text.Wrap
            maximumLineCount: 1
        }
        Text {
            id: descriptionLabel
            width: parent.width
            text: description
            elide: Text.ElideRight
            wrapMode: Text.Wrap
            maximumLineCount: 4

            TooltipArea {
                id: tooltip
                anchors.fill: parent
                text: description
            }
        }
    }

    InstallButton {
        anchors {
            bottom: hr.top
            bottomMargin: 3
            horizontalCenter: preview.horizontalCenter
        }
    }

    Rectangle {
        id: hr
        width: parent.width
        height: 1
        anchors.bottom: parent.bottom
        color: syspal.window
    }
}
