// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.qutim 0.3
import QtWebKit 1.0

Item {
    id: root
    property QtObject session
    property QtObject controller
    property alias preview: chatController.preview

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: Math.max(parent.width, webView.width)
        contentHeight: Math.max(parent.height, webView.height)
        pressDelay: 200
        clip: true
        onHeightChanged: flickable.scrollToBottom()
        
        function scrollToBottom() {
            flickable.contentY = flickable.contentHeight - flickable.height;
        }
        
        WebView {
            id: webView
            preferredWidth: flickable.width
            preferredHeight: flickable.height
            onAlert: console.log(message)
            settings.defaultFontSize: chatController.fontSize
            settings.standardFontFamily: chatController.fontFamily
            settings.fixedFontFamily: chatController.fontFamily
            settings.serifFontFamily: chatController.fontFamily
            settings.sansSerifFontFamily: chatController.fontFamily
            javaScriptWindowObjects: ChatController {
                id: chatController
                WebView.windowObjectName: "client"
                webView: webView
                session: root.session
            }
            Connections {
                target: chatController.session
                ignoreUnknownSignals: true
                onMessageAppended: chatController.append(message)
            }
            onHeightChanged: flickable.scrollToBottom()
        }
    }
    ScrollDecorator {
        flickableItem: flickable
    }
    Component.onCompleted: {
        console.log("Component.onCompleted", chatController);
        root.controller = chatController
        console.log("Component.onCompleted", root.controller);
    }
}
