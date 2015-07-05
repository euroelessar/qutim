import QtQuick 2.1
import QtQuick.Window 2.1
import org.qutim.kineticpopups 0.4
import "logic.js" as Logic

Window {
    id: root

    property Component popupComponent

    readonly property real screenHeight: Screen.desktopAvailableHeight
    readonly property real screenWidth: Screen.desktopAvailableWidth

    onScreenHeightChanged: Logic.relayoutItems()
    onScreenWidthChanged: Logic.relayoutItems()

    Timeout {
        id: timeout
    }

    Component {
        id: animationComponent
        
        Item {
            id: popup

            property Window window
            property real finalX
            property real finalY
            
            function move(x, y) {
                if (moveAnimation.running)
                    moveAnimation.stop();
                
                finalX = x;
                finalY = y;
                moveAnimation.start();
            }
            
            function show() {
                hideAnimation.stop();
                showAnimation.start();
            }
            
            function hide() {
                showAnimation.stop();
                hideAnimation.start();
            }
            
            SequentialAnimation {
                id: showAnimation
                
                PropertyAction {
                    target: window
                    property: "visible"
                    value: true
                }
                NumberAnimation {
                    target: window
                    property: "opacity"
                    duration: Logic.DURATION
                    to: 1.0
                }
            }
            
            SequentialAnimation {
                id: hideAnimation
                
                NumberAnimation {
                    target: window
                    property: "opacity"
                    duration: Logic.DURATION
                    to: 0.0
                }
                PropertyAction {
                    target: window
                    property: "visible"
                    value: false
                }
                ScriptAction {
                    script: {
                        popup.window.destroy()
                        popup.destroy()
                    }
                }
            }

            ParallelAnimation {
                id: moveAnimation
                
                NumberAnimation {
                    target: window
                    property: "x"
                    duration: Logic.DURATION
                    to: popup.finalX
                }
                NumberAnimation {
                    target: window
                    property: "y"
                    duration: Logic.DURATION
                    to: popup.finalY
                }
            }

            Timer {
                interval: {
                    if(timeout.timeout > 0)
                        return timeout.timeout
                    else stop();
                }
                repeat: false
                running: true
                onTriggered: Logic.removePopup(popup)
            }

            Component.onCompleted: {
                window = popupComponent.createObject(null)
            }
        }
    }

    NotifyList {
        id: list

        onNotifyAdded: Logic.addPopup(notify)
        onNotifyRemoved: Logic.removePopup(notify)
    }

    Component.onCompleted: Logic.init(list.notifies)
    Component.onDestruction: Logic.deinit()
}
