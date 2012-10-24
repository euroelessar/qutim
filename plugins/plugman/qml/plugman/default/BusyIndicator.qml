import QtQuick 1.1

Item {
	id: indicator

	property alias running : rotation.running

    property int __leafCount: 12
    property real __rotation: 360 / __leafCount
    property bool __inverted: false
    property int __period: 800
    property string __size: "medium"

    implicitWidth: __size === "small" ? 16 : __size === "medium" ? 32 : 48;
	implicitHeight: implicitWidth

	Image {
		id: spinner
        source: "images/spinner/busy_indicator_" + indicator.implicitWidth + (__inverted ? "_inverted" : "") + ".png"
		smooth: true
	}

	Timer {
		id: rotation
        interval: __period / __leafCount
		running: true
		repeat: true
		onTriggered: {
            spinner.rotation = spinner.rotation + __rotation;
		}
	}
}
