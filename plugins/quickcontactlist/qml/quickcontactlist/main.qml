import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.qutim 0.4

Rectangle {
    width: 200
    height: 600
    
    SystemPalette {
        id: palette
    }
    
    color: palette.window
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        ContactListToolBar {
            Layout.fillWidth: true
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            
            ContactsView {
            }
        }

        Button {
            Layout.fillWidth: true
            iconName: "im-user-online-blab-alb"
            text: "Online"
        }
    }
}
