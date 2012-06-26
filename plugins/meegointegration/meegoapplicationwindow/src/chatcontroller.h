#ifndef MEEGOINTEGRATION_CHATCONTROLLER_H
#define MEEGOINTEGRATION_CHATCONTROLLER_H

#include <qutim/chatsession.h>
#include "../../../adiumwebview/lib/webkitmessageviewcontroller.h"
#include "chatviewstyle.h"

namespace MeegoIntegration {

class ChatController : public WebKitMessageViewController
{
	Q_OBJECT
	Q_PROPERTY(QString fontFamily READ fontFamily NOTIFY fontFamilyChanged)
	Q_PROPERTY(int fontSize READ fontSize NOTIFY fontSizeChanged)
	Q_PROPERTY(QObject* webView READ webView WRITE setWebView NOTIFY webViewChanged)
	Q_PROPERTY(MeegoIntegration::ChatViewStyle *style READ style CONSTANT)
public:
	ChatController();
	~ChatController();
	
	QString fontFamily() const;
	int fontSize() const;
	QObject *webView() const;
	void setWebView(QObject *webView);
	ChatViewStyle *style() const;
	
public slots:
	void fixFlickable(QObject *object);
	void append(const qutim_sdk_0_3::Message &message);
	void handleElement(const QWebElement &element);
	
protected:
	virtual void appendNick(const QVariant &nick);
	virtual void appendText(const QVariant &text);
	virtual void setDefaultFont(const QString &family, int size);

signals:
	void nickAppended(const QString &nick);
	void textAppended(const QString &text);
	void fontFamilyChanged(const QString &fontFamily);
	void fontSizeChanged(int fontSize);
	void webViewChanged(QObject *webView);
	
private:
	QString m_fontFamily;
	int m_fontSize;
	QObject *m_webView;
	ChatViewStyle *m_viewStyle;
};

} // namespace MeegoIntegration

#endif // MEEGOINTEGRATION_CHATCONTROLLER_H
