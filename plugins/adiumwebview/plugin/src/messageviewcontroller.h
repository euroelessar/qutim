#ifndef ADIUMWEBVIEW_MESSAGEVIEWCONTROLLER_H
#define ADIUMWEBVIEW_MESSAGEVIEWCONTROLLER_H

#include <QObject>
#include <QVariant>
#include <QPointer>
#include "../../lib/webkitmessageviewstyle.h"
#include <qutim/message.h>

namespace AdiumWebView {

class MessageViewController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qutim_sdk_0_3::ChatSession* session READ session WRITE setSession NOTIFY sessionChanged)
public:
    explicit MessageViewController(QObject *parent = 0);
	virtual ~MessageViewController();
	
	void setSession(qutim_sdk_0_3::ChatSession *session);
	qutim_sdk_0_3::ChatSession *session() const;

	void clearChat();
	WebKitMessageViewStyle *style();
	void setDefaultFont(const QString &family, int size);
	QString defaultFontFamily() const;
	int defaultFontSize() const;

public slots:
    void append(const qutim_sdk_0_3::Message &msg);
	void debugLog(const QString &message);
	void appendNick(const QVariant &nick);
	void contextMenu(const QVariant &nickVar);
	void appendText(const QVariant &text);
    void setLoading(bool loading);
    
signals:
    void javaScriptRequest(const QString &script);
    void htmlRequested(const QString &html);
    void topicRequested(const QString &topic);
    void clearFocusRequested();
    void fontRequested(const QString &family, int size);
    void sessionChanged();
	
protected:
    void setHtml(const QString &html);
    void evaluateJavaScript(const QString &script);

	void clearFocusClass();
	bool isContentSimiliar(const qutim_sdk_0_3::Message &a, const qutim_sdk_0_3::Message &b);
	void loadSettings(bool onFly);
	
private slots:
	void onSettingsSaved();
	void onLoadFinished();
	void onTopicChanged(const QString &topic);
	void updateTopic();
	
private:
	QPointer<qutim_sdk_0_3::ChatSession> m_session;
	QString m_styleName;
	WebKitMessageViewStyle m_style;
	bool m_isLoading;
	bool m_isPreview;
	qutim_sdk_0_3::Message m_last;
	qutim_sdk_0_3::Message m_topic;
};

} // namespace AdiumWebView

#endif // ADIUMWEBVIEW_MESSAGEVIEWCONTROLLER_H
