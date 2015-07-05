import QtQuick 1.0
import QtWebKit 1.0
//import org.adium 0.1

//WebChatView {
//	page: adiumData
//}

Flickable {
	id: root
	property alias html: webView.html
	contentWidth: root.width
	contentHeight: webView.contentsSize.height - 20
	pressDelay: 200
	
	WebView {
		id: webView
		width: parent.width
		html: adiumData.baseHtml
		focus: true
		preferredWidth: root.width
		preferredHeight: root.height
		onLoadFinished: {
			var scripts = adiumData.scripts;
			for (var i = 0; i < scripts.length; ++i)
				evaluateJavaScript(scripts[i]);
		}
		onHeightChanged: console.log(webView.height, webView.contentsSize.height, webView.preferredHeight)
	}
}
