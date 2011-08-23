import QtQuick 1.0
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3

Page {
	id: root
	property variant chat
	ListView {
		id: listView
		model: ChatChannelModel {
			
		}
		anchors.fill: parent
		delegate: ListDelegate {
			onClicked: {
				channel.active = true
			}
		}
	}
}
