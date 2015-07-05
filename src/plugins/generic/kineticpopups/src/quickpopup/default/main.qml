import QtQuick 2.1

Controller {
    popupComponent: PopupBase {
        id: window
        
        BorderImage {
            id: borderImage
            anchors.fill: window.contentItem
            border.left: 5; border.top: 5
            border.right: 5; border.bottom: 5
            source: "images/background.png"
            opacity: 0.8
        }
    }
}
