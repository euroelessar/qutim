#include "chatcontroller.h"
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QGraphicsObject>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

ChatController::ChatController()
	: WebKitMessageViewController(false),
	  m_webView(0),
	  m_viewStyle(new ChatViewStyle(WebKitMessageViewController::style(), this))
{
}

ChatController::~ChatController()
{
}

QString ChatController::fontFamily() const
{
	return m_fontFamily;
}

int ChatController::fontSize() const
{
	return m_fontSize;
}

QObject *ChatController::webView() const
{
	return m_webView;
}

void ChatController::setWebView(QObject *webView)
{
	if (m_webView != webView) {
		m_webView = webView;
		emit webViewChanged(webView);
		QString script = QLatin1String("client.handleElement(document.querySelector('*'));");
        QMetaObject::invokeMethod(m_webView, "evaluateJavaScript",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, script));
	}
}

ChatViewStyle *ChatController::style() const
{
	return m_viewStyle;
}

void ChatController::fixFlickable(QObject *object)
{
	QGraphicsObject *graphicsObject = qobject_cast<QGraphicsObject*>(object);
	graphicsObject->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
}

void ChatController::append(const Message &message)
{
	appendMessage(message);
}

void ChatController::handleElement(const QWebElement &element)
{
	setPage(element.webFrame()->page());
}

void ChatController::appendNick(const QVariant &nick)
{
	emit nickAppended(nick.toString());
}

void ChatController::appendText(const QVariant &text)
{
	emit textAppended(text.toString());
}

void ChatController::setDefaultFont(const QString &family, int size)
{
	QFontInfo info(QFont(family, size));
	size = info.pixelSize();
	if (m_fontFamily != family) {
		m_fontFamily = family;
		emit fontFamilyChanged(family);
	}
	if (m_fontSize != size) {
		m_fontSize = size;
		emit fontSizeChanged(size);
	}
}

} // namespace MeegoIntegration
