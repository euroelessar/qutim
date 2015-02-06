import QtQuick 2.0
import QtWebEngine 1.0
import org.qutim.quickchat 0.4

WebEngineView {
    id: webEngineView

    WebEngineView {
        id: linksHandler
        enabled: false
        visible: false

        onNavigationRequested: {
            request.action = WebEngineView.IgnoreRequest;
            Qt.openUrlExternally(request.url);
        }
    }

    property alias session: controller.session

    signal appendTextRequested(string text)
    signal appendNickRequested(string nick)

    ControlledMenu {
        id: menu
    }

    onLoadingChanged: {
        if (loadRequest.status === WebEngineView.LoadSucceededStatus) {
            controller.loading = false;

            var scripts = controller.scripts;
            controller.scripts = [];

            for (var i = 0; i < scripts.length; ++i) {
                runJavaScript(scripts[i]);
            }
        }
    }
    onNavigationRequested: {
        if (request.navigationType === WebEngineView.LinkClickedNavigation) {
            request.action = WebEngineView.IgnoreRequest;
            Qt.openUrlExternally(request.url);
        }
    }
    onJavaScriptConsoleMessage: {
        console.log('WebKit', message, lineNumber, sourceID);
    }
    onNewViewRequested: request.openIn(linksHandler)

    ChatController {
        id: controller
        property variant scripts: []
        property bool loading: false

        onHtmlRequested: {
            loading = true;
            webEngineView.loadHtml(html, baseUrl);
            scripts = [];
        }
        onJavaScriptRequested: {
            if (loading) {
                scripts.push(script);
            } else {
                webEngineView.runJavaScript(script);
            }
        }
        onAppendTextRequested: webEngineView.appendTextRequested(text)
        onAppendNickRequested: webEngineView.appendNickRequested(nick)
        onMenuRequested: {
            menu.controller = owner;
            menu.popup();
        }
    }
}
