import QtQuick 2.1
import "../default" as Default

Default.Controller {
    popupComponent: Default.PopupBase {
        id: window
        
        Rectangle {
            anchors.fill: window.contentItem
            radius: 10
            smooth: true
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#6495ED" }
                GradientStop { position: 0.33; color: "#191970" }
                GradientStop { position: 1.0; color: "#000080" }
            }
        }
    }
}
