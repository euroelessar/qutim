import QtQuick 1.0
import com.nokia.meego 1.0
import "Utils.js" as Utils

TabButton {
	id: root
    property string platformIconId
	iconSource: Utils.handleIconSource(platformIconId)
}
