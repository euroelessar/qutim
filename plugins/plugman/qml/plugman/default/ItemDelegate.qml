import QtQuick 1.1
import QtDesktop 0.1
import org.qutim.plugman 0.3

Item {
    id: root
    height: 120
    width: parent ? parent.width : 300

    Image {
        id: preview

        width: 100
        height: Math.min(sourceSize.height, 100)

        source: imageSource ? imageSource : "images/utilities-file-archiver.png"
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
            leftMargin: 5
            rightMargin: 5
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

        InstallButton {
        }
    }
}
