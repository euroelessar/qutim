import QtQuick 1.1

Item {
	id: indicator

	property alias running : rotation.running
    property int period: 800
    property string size: "medium"
    property bool inverted: false

    property int __leafCount: 12
    property real __rotation: 360 / __leafCount

    implicitWidth: size === "small" ? 16 : size === "medium" ? 32 : 48;
	implicitHeight: implicitWidth

	Image {
		id: spinner
        source: "images/spinner/busy_indicator_" + indicator.implicitWidth + (inverted ? "_inverted" : "") + ".png"
		smooth: true
	}

	Timer {
		id: rotation
        interval: period / __leafCount
		running: true
		repeat: true
		onTriggered: {
            spinner.rotation = spinner.rotation + __rotation;
		}
	}
}
