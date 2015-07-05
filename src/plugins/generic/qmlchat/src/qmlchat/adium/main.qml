import QtQuick 2.2
import org.qutim 0.4
import org.qutim.adiumwebview 0.4
import QtQuick.Controls 1.1

Item {
    id: main
//    color: "red"

    property QtObject controller

    ScrollView {
        anchors.fill: parent
        AdiumView {
            id: adiumView
            session: controller ? controller.session : null
        }
    }

    Connections {
        target: controller
        onMessageAppended: adiumView.appendMessage(message)
        onMessageDelivered: adiumView.deliverMessage(messageId)
    }
}
