/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef WEBKITMESSAGEVIEWCONTROLLER_H
#define WEBKITMESSAGEVIEWCONTROLLER_H

#include <QObject>
#include <QWebElement>
#include <QWebPage>
#include <qutim/chatsession.h>
#include "webkitmessageviewstyle.h"

class ADIUMWEBVIEW_EXPORT WebViewLoaderLoop : public QObject
{
    Q_OBJECT
public:
	WebViewLoaderLoop();
	~WebViewLoaderLoop();
	
	void addPage(QWebPage *page, const QString &html);
	
private slots:
	void onPageLoaded();
	void onPageDestroyed();
	
private:
	QList<QWeakPointer<QWebPage> > m_pages;
	QStringList m_htmls;
};

class ADIUMWEBVIEW_EXPORT WebKitMessageViewController : public QObject
{
	Q_OBJECT
	Q_PROPERTY(qutim_sdk_0_3::ChatSession* session READ session WRITE setSession NOTIFY sessionChanged)
	Q_PROPERTY(bool preview READ isPreview WRITE setPreview NOTIFY previewChanged)

public:
	WebKitMessageViewController(bool isPreview);
	~WebKitMessageViewController();
	
	qutim_sdk_0_3::ChatSession *session() const;
	void setSession(qutim_sdk_0_3::ChatSession *session);
	WebKitMessageViewStyle *style();
	void appendMessage(const qutim_sdk_0_3::Message &msg);
	bool eventFilter(QObject *obj, QEvent *);
	bool isPreview() const;
	void setPreview(bool preview);
	
public slots:
	void clearChat();
	QVariant evaluateJavaScript(const QString &script);
	bool zoomImage(QWebElement elem);
	void debugLog(const QString &message);
	virtual void appendNick(const QVariant &nick) = 0;
	void contextMenu(const QVariant &nickVar);
	virtual void appendText(const QVariant &text) = 0;
	void setTopic();
	virtual void setDefaultFont(const QString &family, int size) = 0;
	
private slots:
	void onJavaScriptRequest(const QString &javaScript, QVariant *variant);
	
protected:
	void setPage(QWebPage *page);
	void clearFocusClass();
	bool isContentSimiliar(const qutim_sdk_0_3::Message &a, const qutim_sdk_0_3::Message &b);
	void loadSettings(bool onFly);
	void loadHistory();
	
signals:
	void sessionChanged(qutim_sdk_0_3::ChatSession *session);
	void previewChanged(bool preview);
	
private slots:
	void onSettingsSaved();
	void onLoadFinished();
	void onTopicChanged(const QString &topic);
	void updateTopic();
	void onContentsChanged();
	void onObjectCleared();
	void onLinkClicked(const QUrl &url);
	
private:
	void init();
	
	QWebPage *m_page;
	QWeakPointer<qutim_sdk_0_3::ChatSession> m_session;
	QString m_styleName;
	WebKitMessageViewStyle m_style;
	bool m_isLoading;
	bool m_isPreview;
	QStringList m_pendingScripts;
	qutim_sdk_0_3::Message m_last;
	qutim_sdk_0_3::Message m_topic;
};

#endif // WEBKITMESSAGEVIEWCONTROLLER_H
