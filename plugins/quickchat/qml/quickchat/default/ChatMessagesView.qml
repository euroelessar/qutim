import QtQuick 2.0
import QtWebEngine 1.0
import org.qutim.quickchat 0.4

WebEngineView {
    id: webEngineView

    property alias session: controller.session
    url: 'http://qutim.org/'

    signal appendTextRequested(string text)
    signal appendNickRequested(string nick)

    onLoadingChanged: {
        console.log('loading changed',
                    loadRequest.status,
                    loadRequest.url,
                    JSON.stringify(loadRequest.errorString),
                    loadRequest.errorCode,
                    loadRequest.errorDomain
                    );

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

    ChatController {
        id: controller
        property variant scripts: {[]}
        property bool loading: false

        onHtmlRequested: {
            console.log("html load request", baseUrl);
            loading = true;
            webEngineView.loadHtml(html, baseUrl);
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
    }
}
