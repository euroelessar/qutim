#ifndef MEEGOINTEGRATION_CHATCONTROLLER_H
#define MEEGOINTEGRATION_CHATCONTROLLER_H

#include "chatchannel.h"
#include "scriptclient.h"
#include "../../adiumwebview/lib/webkitmessageviewstyle.h"
#include <QQuickTextDocument>

namespace QuickChat {

class ChatController : public QObject
{
	Q_OBJECT
    Q_PROPERTY(QuickChat::ChatChannel* session READ session WRITE setSession NOTIFY sessionChanged)
//    Q_PROPERTY(QQuickTextDocument* document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(QString fontFamily READ fontFamily NOTIFY fontFamilyChanged)
    Q_PROPERTY(int fontSize READ fontSize NOTIFY fontSizeChanged)
public:
	ChatController();
	~ChatController();

    QString id() const;
	
	QString fontFamily() const;
    int fontSize() const;

    ChatChannel *session() const;
    void setSession(ChatChannel *session);

protected:
    void onMessageAppended(const qutim_sdk_0_3::Message &message);
    void clearChat();
    WebKitMessageViewStyle *style();
    bool eventFilter(QObject *obj, QEvent *);

public slots:
    void evaluateJavaScript(const QString &script);
    void debugLog(const QString &message);
    void appendNick(const QString &nick);
    void contextMenu(const QString &nickVar);
    void appendText(const QString &text);
    void setTopic(const QString &topic);

protected:
    void clearFocusClass();
    bool isContentSimiliar(const qutim_sdk_0_3::Message &a, const qutim_sdk_0_3::Message &b);
    void loadSettings(bool onFly);
    void loadHistory();
    QString scriptForFontUpdate();

signals:
    void htmlRequested(const QString &html, const QUrl &baseUrl);
    void javaScriptRequested(const QString &script);
    void appendNickRequested(const QString &nick);
    void appendTextRequested(const QString &text);

private slots:
    void onSettingsSaved();

protected:
	virtual void setDefaultFont(const QString &family, int size);

signals:
	void fontFamilyChanged(const QString &fontFamily);
	void fontSizeChanged(int fontSize);
	void webViewChanged(QObject *webView);
    void sessionChanged(ChatChannel *session);

private:
	QString m_fontFamily;
    int m_fontSize;

    QString m_id;
    QSharedPointer<ScriptClient> m_client;
    ChatChannel *m_session;
    QString m_styleName;
    WebKitMessageViewStyle m_style;
    bool m_isLoading;
    bool m_isPreview;
    QStringList m_pendingScripts;
    qutim_sdk_0_3::Message m_last;
    qutim_sdk_0_3::Message m_topic;
};

} // namespace QuickChat

#endif // MEEGOINTEGRATION_CHATCONTROLLER_H
