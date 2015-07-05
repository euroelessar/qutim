import QtQuick 2.1
import "../default" as Default

Default.Controller {
    popupComponent: Default.PopupBase {
        id: window
        
        textColor: "black"
        textStyle: Text.Normal
        
        Rectangle {
            anchors.fill: window.contentItem
            radius: 10
            smooth: true
            border.color: "#FFFAFA"
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#EEE9E9" }
                GradientStop { position: 1.0; color: "#CDC9C9" }
            }

        }
    }
}
