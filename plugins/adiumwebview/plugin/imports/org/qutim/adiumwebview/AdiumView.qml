import QtQuick 2.2
import QtWebKit 3.0
import QtWebKit.experimental 1.0
import org.qutim.adiumwebview 0.4
import org.qutim 0.4
import "logic.js" as Logic

// @disable-check M324
WebView {
    id: root
    
    property alias session: controller.session
    pressDelay: 1
//    interactive: false
//    pixelAligned: true
//    contentWidth: parent ? parent.width : undefined
    
    MessageViewController {
        id: controller
        
        onHtmlRequested: {
            Logic.loading = true;
            console.log("onHtmlRequested")
            root.loadHtml(html);
        }
        onJavaScriptRequest: {
            console.log("onJavaScriptRequest", script, root.loading)
            if (Logic.loading) {
                Logic.addScript(script);
            } else {
                root.experimental.evaluateJavaScript(script);
            }
        }
        onTopicRequested: {
            console.log("onTopicRequested", topic)
        }
        onClearFocusRequested: {
            console.log("onClearFocusRequested")
        }
        onFontRequested: {
            console.log("onFontRequested", family, size);
            experimental.preferences.standardFontFamily = family;
            experimental.preferences.fixedFontFamily = family;
            experimental.preferences.serifFontFamily = family;
            experimental.preferences.sansSerifFontFamily = family;
            experimental.preferences.defaultFontSize = size;
        }
    }
    
    // @disable-check M16
    experimental.userScripts: { return [ Qt.resolvedUrl("client.js") ]; }
    // @disable-check M16
    experimental.preferences.navigatorQtObjectEnabled: true
    // @disable-check M16
    experimental.onMessageReceived: {
        var stuff;

        try {
            stuff = JSON.parse(message.data);
        } catch (e) {
            console.log('adiumwebview', 'unknown command', JSON.stringify(message));
            return;
        }

        var name = stuff.name;
        var argument = stuff.data;

        if (name === "debugLog") {
            controller.debugLog(argument.join(', '));
        } else {
            console.log('adiumwebview', 'unknown command:', name, JSON.stringify(argument));
        }
    }
    
    // @disable-check M16
    onLoadingChanged: {
        console.log('loading changed:', loadRequest.status);
        
        if (loadRequest.status === WebView.LoadSucceededStatus) {
            Logic.loading = false;
            var scripts = Logic.takeScripts();
            for (var i = 0; i < scripts.length; ++i)
                experimental.evaluateJavaScript(scripts[i]);
            
            controller.setLoading(false);
        }
    }
    
    function evaluateJavaScript(script) {
        experimental.evaluateJavaScript(script);
    }
    
    function appendMessage(message) {
        console.log('Append message: ', message.text)
        controller.append(message.originalMessage);
    }
    
    function deliverMessage(messageId) {
        console.log('Deliver message: ', messageId)
    }
    
    Component.onCompleted: {
        console.log('adiumwebview', 'onCompleted');
        Logic.loading = true;
    }
}
